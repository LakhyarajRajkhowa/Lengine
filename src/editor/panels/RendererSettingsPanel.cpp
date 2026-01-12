#include "RendererSettingsPanel.h"

using namespace Lengine;

void RendererSettingsPanel::OnImGuiRender()
{
    ImGui::Begin("Renderer Settings");

    // HDR
    if (ImGui::Button(m_Settings.HDR ? "HDR: ON" : "HDR: OFF"))
        m_Settings.HDR = !m_Settings.HDR;

    if (m_Settings.HDR)
    {
        ImGui::SliderFloat("Exposure", &m_Settings.exposure, 0.01f, 5.0f);

        if (ImGui::Button(m_Settings.enableBloom ? "Bloom: ON" : "Bloom: OFF"))
            m_Settings.enableBloom = !m_Settings.enableBloom;

        if (m_Settings.enableBloom)
            ImGui::SliderFloat("Bloom Blur", &m_Settings.bloomBlur, 0.2f, 10.0f);
    }

    ImGui::Separator();

    // MSAA (with reload marking)
    if (ImGui::Button(m_Settings.MSAA ? "MSAA: ON" : "MSAA: OFF"))
    {
        m_Settings.MSAA = !m_Settings.MSAA;
        m_Settings.needsReload = true;
    }

    if (m_Settings.MSAA)
    {
        static const int samples[] = { 2, 4, 8 };
        static const char* labels[] = { "2x", "4x", "8x" };

        int index = (m_Settings.msaaSamples == 8) ? 2 :
            (m_Settings.msaaSamples == 4) ? 1 : 0;

        if (ImGui::Combo("MSAA Samples", &index, labels, 3))
        {
            m_Settings.msaaSamples = samples[index];
            m_Settings.needsReload = true;
        }
    }

    ImGui::End();
}
