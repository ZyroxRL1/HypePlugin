#pragma once
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "version.h"
#include <chrono>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class SpeedTrainer : public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{
public:
    void onLoad() override;
    void onUnload() override;
    void Render(CanvasWrapper canvas);
    void OnGoalScored(std::string eventName);

    void RenderSettings() override;
    std::string GetPluginName() override { return "Hype Plugin"; }
    void SetImGuiContext(uintptr_t ctx) override;

private:
    std::string currentHypeText = "";
    bool isShowing = false;
    float displayTime = 3.0f;
    std::chrono::steady_clock::time_point goalScoredTime;

    char hypeTexts[8][128] = {
        "OTHER TEAM GOT SHIT ON",
        "TOO EASY BRO",
        "GOAT IN THE BUILDING",
        "THEY CANT STOP ME",
        "ACTUAL DEMON",
        "GO NEXT LOSERS",
        "NOT EVEN TRYING",
        "RIP THEIR MENTAL"
    };

    char quickChat[4][128] = {
        "What a save! xD",
        "Too easy!",
        "Calculated.",
        "No problem."
    };

    float textColorF[3] = { 1.0f, 0.65f, 0.0f };
};