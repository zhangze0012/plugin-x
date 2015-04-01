
#include <stdlib.h>
#include <string>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <unordered_map>

#include "PluginFactory.h"
#include "PluginProtocol.h"
#include "PluginProtocol.h"
#include "ProtocolAds.h"
#include "ProtocolAnalytics.h"
#include "ProtocolIAP.h"
#include "PluginMap.h"
#include "util.h"

using namespace cocos2d::plugin;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::winrtInterface;

static PluginFactory* s_pFactory;

PluginFactory::PluginFactory() {
}

PluginFactory::~PluginFactory() {
}

void PluginFactory::setDispatcher(Windows::UI::Core::CoreDispatcher^ d) {
    dispatcher = d;
}

PluginFactory* PluginFactory::getInstance() {
	if (s_pFactory == nullptr) {
		s_pFactory = new PluginFactory();
	}
	return s_pFactory;
}

void PluginFactory::purgeFactory() {
	if (s_pFactory != nullptr) {
		delete s_pFactory;
		s_pFactory = nullptr;
	}
}

typedef HRESULT(WINAPI *DllActivationFactoryPtr)(HSTRING, IActivationFactory **);
// TODO should return correct protocol object
PluginProtocol* PluginFactory::createPlugin(const char* name) {
	//ComPtr<IProtocol> Protocol;
	std::wstring suffix;
#if WINAPI_FAMILY==WINAPI_FAMILY_PC_APP
	suffix = L".Windows.dll";
#else
	suffix = L".WindowsPhone.dll";
#endif
	std::wstring wname(name, name + strlen(name));
	HMODULE module = LoadPackagedLibrary((wname + suffix).c_str(), 0);
	if (module == nullptr)
	{
		OutputDebugStringA("Failed to load Plugin ");
		OutputDebugStringA(name);
		return nullptr;
	}
	DllActivationFactoryPtr _DllActivationFactory = (DllActivationFactoryPtr)GetProcAddress(module, "DllGetActivationFactory");
	if (!_DllActivationFactory)
	{
		OutputDebugStringA("Failed to load Plugin ActivationFactory entrypoint");
		return nullptr;
	}

	HString Plugin, PluginAnalytics;
	Plugin.Set((wname + L"." + wname).c_str());
	ComPtr<IActivationFactory>factoryPlugin;
	_DllActivationFactory(Plugin.Get(), factoryPlugin.GetAddressOf());

	if (factoryPlugin.Get() == nullptr)
	{
		OutputDebugStringW((L"Plugin failed to implement class " + wname + L"." + wname).c_str());
		return nullptr;
	}
	ComPtr<IInspectable> instance;
	factoryPlugin.Get()->ActivateInstance(instance.GetAddressOf());
	if (instance.Get() == nullptr)
	{
		OutputDebugStringA("Failed to create instance of plugin");
		return nullptr;
	}
	ComPtr<IProtocol> base;
	instance.As(&base);
	if (base.Get() == nullptr)
	{
		OutputDebugStringA("Plugin Fails to Implement IProtocol");
		return nullptr;
	}
	// Try all protocols to see which one implements
	ComPtr<IProtocolAnalytics> analytics;
	instance.As(&analytics);
	if (analytics.Get() != nullptr)
	{
		ProtocolAnalytics *out = new ProtocolAnalytics();
		PluginMap::mapIProtocol[out] = reinterpret_cast<winrtInterface::IProtocol^>(base.Detach());
		PluginMap::mapIProtocolAnalytics[out] = dynamic_cast<winrtInterface::IProtocolAnalytics^>(PluginMap::mapIProtocol[out]);
		return out;
	}
	else {
		ComPtr<IProtocolIAP> iap;
		instance.As(&iap);
		if (iap.Get() != nullptr) {
			ProtocolIAP *out = new ProtocolIAP();
			PluginMap::mapIProtocol[out] = reinterpret_cast<winrtInterface::IProtocol^>(base.Detach());
			PluginMap::mapIProtocolIAP[out] = dynamic_cast<winrtInterface::IProtocolIAP^>(PluginMap::mapIProtocol[out]);
            auto iDispatcher = (ABI::Windows::UI::Core::ICoreDispatcher*)dispatcher;
            iap->setDispatcher(iDispatcher);
            // register protocol for plugin event
            PluginMap::mapIProtocolIAP[out]->OnPayResult += ref new winrtInterface::OnPayResultHandler([out](winrtInterface::PayResultCodeEnum ret, Platform::String^ msg) {
                out->onPayResult((PayResultCode)ret, pluginx::util::PlatformStringToStdString(msg).c_str());
            });
			return out;
		}
	}
	OutputDebugStringA("Protocol Failed to Load ...");
	return nullptr;
}
