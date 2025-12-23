#include "LoadingSystem.h"
#include <algorithm>

namespace Lengine {


    void LoadingSystem::progressBarUI(
        float progress,
        const LoadingUIConfig& cfg
    )
    {
        progress = std::clamp(progress, 0.0f, 1.0f);

        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(cfg.size, ImGuiCond_Always);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse;

        bool open = false;

        if (cfg.modal)
        {
            ImGui::OpenPopup(cfg.title);
            open = ImGui::BeginPopupModal(cfg.title, nullptr, flags);
        }
        else
        {
            open = ImGui::Begin(cfg.title, nullptr, flags);
        }

        if (!open)
            return;

        // -------- UI --------
        if (cfg.message.c_str())
            ImGui::TextWrapped("%s", cfg.message.c_str());

        char overlay[32];
        std::snprintf(overlay, sizeof(overlay), "%d%%", int(progress * 100));

        ImGui::ProgressBar(progress, ImVec2(-1, 0), overlay);

        // -------- END --------
        if (cfg.modal)
            ImGui::EndPopup();
        else
            ImGui::End();
    }
}
