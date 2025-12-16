#include "PerformancePanel.h"

using namespace Lengine;

PerformancePanel::PerformancePanel() {
	
}

void PerformancePanel::OnImGuiRender()
{
    ImGui::Begin("Performance");

    ImGui::Checkbox("Limit FPS", &limitFPS);
    ImGui::SliderInt("Target FPS", &targetFPS, 30, 240);

    FrameStats stats = LimitFPS(targetFPS, limitFPS);
    deltaTime = stats.deltaTime;

    // --- FPS smoothing ---
    constexpr float smoothing = 0.001f; // lower = smoother
    if (smoothedFPS == 0.0f)
    {
        smoothedFPS = stats.fps;
        smoothedMs = stats.msPerFrame;
    }
    else
    {
        smoothedFPS += (stats.fps - smoothedFPS) * smoothing;
        smoothedMs += (stats.msPerFrame - smoothedMs) * smoothing;
    }

    ImGui::Text("FPS: %.1f", smoothedFPS);
    ImGui::Text("Frame Time: %.2f ms", smoothedMs);

    ImGui::Separator();
    ImGui::End();
}
