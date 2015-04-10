
// Only needed in order to export tne interface classes in winmd, can be 
// deleted if a better way is found.

#include "pch.h"
#include "IProtocolSocial.h"

using namespace cocosPluginWinrtBridge;

namespace cocosPluginWinrtBridge {
    public ref class ProtocolSocialImpl sealed : public cocosPluginWinrtBridge::IProtocolSocial {
    public:

        virtual Platform::String^ getPluginVersion() {
            return L"";
        }

        virtual Platform::String^ getSDKVersion() {
            return L"";
        }

        virtual void setDebugMode(bool bDebug) {
            return;
        }

        virtual void callFuncWithParam(Platform::String^ funcName, Windows::Foundation::Collections::IVector<IPluginParam^>^ params) {
            return;
        }

        virtual Platform::String^ callStringFuncWithParam(Platform::String^ funcName, Windows::Foundation::Collections::IVector<IPluginParam^>^ params) {
            return L"";
        }

        virtual int callIntFuncWithParam(Platform::String^ funcName, Windows::Foundation::Collections::IVector<IPluginParam^>^ params) {
            return 0;
        }

        virtual bool callBoolFuncWithParam(Platform::String^ funcName, Windows::Foundation::Collections::IVector<IPluginParam^>^ params) {
            return false;
        }

        virtual float callFloatFuncWithParam(Platform::String^ funcName, Windows::Foundation::Collections::IVector<IPluginParam^>^ params) {
            return 0;
        }

        virtual void configDeveloperInfo(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ devInfo) {
            return;
        }

        virtual void submitScore(Platform::String^ leadboardID, int64 score) {
            return;
        }

        virtual void showLeaderBoard(Platform::String^ leaderboardID) {
            return;
        }

        virtual void unlockAchievement(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ achInfo) {
            return;
        }

        virtual void showAchievements() {
            return;
        }

        virtual event SocialResultHandler^ OnSocialResult;

    };
}