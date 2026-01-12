#pragma once

#include <imgui.h>

#include "../core/settings.h"

namespace Lengine {
    class RendererSettingsPanel
    {
    public:
        RendererSettingsPanel(RenderSettings& settings)
            : m_Settings(settings) {
        }

        void OnImGuiRender();

    private:
        RenderSettings& m_Settings;
    };
    
}

