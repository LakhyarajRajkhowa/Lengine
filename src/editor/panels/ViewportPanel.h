#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#define IMGUI_ENABLE_DOCKING
#define IMGUI_ENABLE_DOCKING_EXTENSION
#include "imgui.h"
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "../graphics/camera/Camera3d.h"
#include "../graphics/frameBuffers/Framebuffer.h"
#include "../graphics/frameBuffers/MSAAFramebuffer.h"
#include "../graphics/frameBuffers/HDRFramebuffer.h"
#include "../graphics/frameBuffers/MSAAHDRFramebuffer.h"


#include "../utils/fps.h"
#include "../utils/imGuiScreens.h"
namespace Lengine {
    enum class ViewportMode {
        first = 0,
        second,
        third,
        count 
    };

    class ViewportPanel {
    public:
        ViewportPanel(Camera3d& camera, glm::vec2 resolution);

        void OnImGuiRender();
        void RenderFullscreen();

        // Access framebuffer for rendering the scene
        Framebuffer& GetFramebuffer() { return Framebuffer; }
        MSAAFramebuffer& GetMSAAFramebuffer() { return MSAAFramebuffer; }
        HDRFramebuffer& GetHDRFramebuffer() { return HDRFramebuffer; }
        MSAAHDRFramebuffer& GetMSAAHDRFramebuffer() { return MSAAHDRFramebuffer; }

        void clearFrame(const glm::vec4& clearColor);
        void ClearFramebuffers();

        // Check if the viewport has resized
        bool IsViewportFocused() const { return m_Focused; }
        bool IsViewportHovered() const { return m_Hovered; }

        ImVec2 GetViewportSize() const { return m_ViewportSize; }
        ImVec2 GetViewportPos() const { return m_ViewportPos; }

        ImVec2 ViewportPanel::getMousePosInViewport() const { return mouseInViewport; }
        ImVec2 ViewportPanel::getMousePosInImage() const { return mouseInImage; }

        bool fixCamera = false;
        bool viewportFullscreen = true;

        
    private:
        Camera3d& camera;

        Framebuffer Framebuffer;
        MSAAFramebuffer MSAAFramebuffer;
        HDRFramebuffer HDRFramebuffer;
        MSAAHDRFramebuffer MSAAHDRFramebuffer;


        float offsetValueX = 0.14f;
        float offsetValueY = -0.1f;
        ImVec2 m_ViewportSize = { 1920, 1080 };
        ImVec2 m_ViewportPos;
        ImVec2 m_LastViewportSize = { -1, -1 };

        ImVec2 mouseInViewport;
        ImVec2 mouseInImage;


        bool m_Focused = false; 
        bool m_Hovered = false;  

        ViewportMode mode = ViewportMode::first;
   

    };

}
