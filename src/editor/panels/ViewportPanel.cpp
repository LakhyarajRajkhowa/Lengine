#include "ViewportPanel.h"
#include <iostream>

using namespace Lengine;

    ViewportPanel::ViewportPanel(Camera3d& cam)
		: m_Framebuffer(1280, 720), camera(cam)
    {
    }

    void ViewportPanel::OnImGuiRender()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        ImGui::Separator();
         
        if (ImGui::Button(camera.isFixed ? "Fix Camera: ON" : "Fix Camera: OFF"))
            camera.isFixed = !camera.isFixed;

        ImGui::SameLine();
        if (ImGui::Button("Fullscreen Mode"))
            viewportFullscreen = true;

        ImGui::Separator();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_ViewportSize = { avail.x, avail.y };
        m_ViewportPos = ImGui::GetWindowPos();

       



        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0)
        {
            if (m_ViewportSize.x != m_LastViewportSize.x ||
                m_ViewportSize.y != m_LastViewportSize.y)
            {
                
                m_LastViewportSize = m_ViewportSize;
            }
        }
        


        GLuint texID = m_Framebuffer.GetColorAttachment();
        

         ImGui::Image(
             (void*)(intptr_t)texID,
             ImVec2(m_ViewportSize.x, m_ViewportSize.y),
             ImVec2(0, 1),
             ImVec2(1, 0)
         );
         m_Focused = ImGui::IsItemFocused();
         m_Hovered = ImGui::IsItemHovered();
         ImVec2 imagePos = ImGui::GetItemRectMin();
         ImVec2 mousePos = ImGui::GetMousePos();
  

         mouseInViewport = { mousePos.x - imagePos.x, mousePos.y - imagePos.y };
    

        ImGui::End();
        ImGui::PopStyleVar();
    }



void ViewportPanel::RenderFullscreen()
{
    camera.isFixed = false;
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking;
        
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("ViewportFullscreen", nullptr, flags);

    // ESC to exit fullscreen
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        viewportFullscreen = false;

    // Draw the framebuffer fullscreen
    GLuint texID = m_Framebuffer.GetColorAttachment();

    ImGui::Image(
        (void*)(intptr_t)texID,
        ImGui::GetContentRegionAvail(),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
    ImGui::PopStyleVar();
}
