#pragma once
#include <imgui/imgui.h>
#include <string>

namespace Lengine {

   

    // LoadingSystem.h
    class LoadingSystem {
    public:
        struct LoadingUIConfig
        {
            const char* title = "Loading";
            std::string message;   // path / asset / phase
            bool showPercentage = true;
            bool modal = true;
            ImVec2 size = ImVec2(360, 0);
        };

        static void progressBarUI(
            float progress,
            const LoadingUIConfig& cfg
        );

    private:
    };


}
