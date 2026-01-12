#include "ViewportPanel.h"
#include <iostream>

using namespace Lengine;

    ViewportPanel::ViewportPanel(Camera3d& cam, glm::vec2 resolution)
		: 
        Framebuffer(resolution.x, resolution.y), camera(cam),
        MSAAFramebuffer(resolution.x, resolution.y),
        HDRFramebuffer(resolution.x, resolution.y),
        MSAAHDRFramebuffer(resolution.x, resolution.y)
    {
    }
    
    void ViewportPanel::clearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void ViewportPanel::ClearFramebuffers() {
        Framebuffer.Bind();
        clearFrame({ 0,0,0,1 });
        Framebuffer.Unbind();

        MSAAFramebuffer.Bind();
        clearFrame({ 0,0,0,1 });
        MSAAFramebuffer.Unbind();

        HDRFramebuffer.Bind();
        clearFrame({ 0,0,0,1 });
        HDRFramebuffer.Unbind();

        MSAAHDRFramebuffer.Bind();
        clearFrame({ 0,0,0,1 });
        MSAAHDRFramebuffer.Unbind();
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
        ImGui::SameLine();

        static const char* viewportModeLabels[] = {
           "First",
           "Second",
           "Third"

        };

        ViewportMode currentMode = mode;
        int currentModeIndex = static_cast<int>(currentMode);
        ImGui::PushItemWidth(140.0f);   

        if (ImGui::Combo("Viewport Mode", &currentModeIndex,
            viewportModeLabels,
            static_cast<int>(ViewportMode::count)))
        {
            camera.controlMode = static_cast<CameraControlMode>(currentModeIndex);
            mode = static_cast<ViewportMode>(currentModeIndex);

        }
        ImGui::PopItemWidth();



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
                camera.setAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
            }
        }
        

        GLuint texID = Framebuffer.GetColorAttachment();
        

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
    GLuint texID = Framebuffer.GetColorAttachment();

    ImGui::Image(
        (void*)(intptr_t)texID,
        ImGui::GetContentRegionAvail(),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
    ImGui::PopStyleVar();
}
