#include "pch.h"
#include "SpeedTrainer.h"
#include <random>
#include <chrono>

BAKKESMOD_PLUGIN(SpeedTrainer, "Hype Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void SpeedTrainer::onLoad()
{
    _globalCvarManager = cvarManager;

    cvarManager->executeCommand("sv_soccar_scoring_message_duration 0");

    gameWrapper->HookEvent(
        "Function TAGame.Ball_TA.OnHitGoal",
        bind(&SpeedTrainer::OnGoalScored, this, std::placeholders::_1)
    );

    gameWrapper->RegisterDrawable(bind(&SpeedTrainer::Render, this, std::placeholders::_1));

    cvarManager->registerNotifier("hype_chat1", [this](std::vector<std::string> args) {
        cvarManager->executeCommand("say " + std::string(quickChat[0]));
        }, "Quick chat 1", PERMISSION_ALL);

    cvarManager->registerNotifier("hype_chat2", [this](std::vector<std::string> args) {
        cvarManager->executeCommand("say " + std::string(quickChat[1]));
        }, "Quick chat 2", PERMISSION_ALL);

    cvarManager->registerNotifier("hype_chat3", [this](std::vector<std::string> args) {
        cvarManager->executeCommand("say " + std::string(quickChat[2]));
        }, "Quick chat 3", PERMISSION_ALL);

    cvarManager->registerNotifier("hype_chat4", [this](std::vector<std::string> args) {
        cvarManager->executeCommand("say " + std::string(quickChat[3]));
        }, "Quick chat 4", PERMISSION_ALL);

    LOG("Hype Plugin geladen!");
}

void SpeedTrainer::onUnload()
{
    cvarManager->executeCommand("sv_soccar_scoring_message_duration 3");
}

void SpeedTrainer::OnGoalScored(std::string eventName)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 7);
    int randomIndex = dis(gen);
    currentHypeText = std::string(hypeTexts[randomIndex]);

    // Sla de huidige tijd op als startpunt
    goalScoredTime = std::chrono::steady_clock::now();
    isShowing = true;

    LOG("GOAL! Hype: {}", currentHypeText);
}

void SpeedTrainer::Render(CanvasWrapper canvas)
{
    if (!isShowing || currentHypeText.empty()) return;

    // Bereken hoeveel seconden geleden de goal was
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - goalScoredTime).count();

    if (elapsed >= displayTime) {
        isShowing = false;
        return;
    }

    auto screenSize = canvas.GetSize();
    float x = screenSize.X / 2.0f - 300.0f;
    float y = screenSize.Y / 2.0f - 30.0f;

    int r = (int)(textColorF[0] * 255);
    int g = (int)(textColorF[1] * 255);
    int b = (int)(textColorF[2] * 255);

    // Fade out laatste 0.5 seconden
    int alpha = 255;
    float remaining = displayTime - elapsed;
    if (remaining < 0.5f) {
        alpha = (int)(remaining / 0.5f * 255);
    }

    // Schaduw
    canvas.SetColor(0, 0, 0, (int)(alpha * 0.8f));
    canvas.SetPosition(Vector2F{ x + 2.0f, y + 2.0f });
    canvas.DrawString(currentHypeText, 4.0f, 4.0f);

    // Hoofd tekst
    canvas.SetColor(r, g, b, alpha);
    canvas.SetPosition(Vector2F{ x, y });
    canvas.DrawString(currentHypeText, 4.0f, 4.0f);
}

void SpeedTrainer::SetImGuiContext(uintptr_t ctx)
{
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void SpeedTrainer::RenderSettings()
{
    ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.0f, 1.0f), "=== HYPE PLUGIN INSTELLINGEN ===");
    ImGui::Spacing();

    ImGui::Text("Hype teksten bij goals:");
    ImGui::Separator();
    for (int i = 0; i < 8; i++) {
        std::string label = "Tekst " + std::to_string(i + 1);
        ImGui::InputText(label.c_str(), hypeTexts[i], 128);
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Hoe lang tekst zichtbaar (seconden):");
    ImGui::SliderFloat("Display tijd", &displayTime, 1.0f, 10.0f);

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Tekstkleur:");
    ImGui::ColorEdit3("Kleur", textColorF);

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.0f, 1.0f), "=== CUSTOM QUICK CHAT ===");
    ImGui::Text("Typ in console: hype_chat1, hype_chat2, etc.");
    ImGui::Spacing();

    for (int i = 0; i < 4; i++) {
        std::string label = "Quick Chat " + std::to_string(i + 1) + " (hype_chat" + std::to_string(i + 1) + ")";
        ImGui::InputText(label.c_str(), quickChat[i], 128);
    }

    ImGui::Spacing();

    if (ImGui::Button("Test Hype Tekst!")) {
        currentHypeText = std::string(hypeTexts[0]);
        goalScoredTime = std::chrono::steady_clock::now();
        isShowing = true;
    }

    if (ImGui::Button("Reset tekst")) {
        currentHypeText = "";
        isShowing = false;
    }
}