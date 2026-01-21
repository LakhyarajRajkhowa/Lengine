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
#include "../graphics/frameBuffers/SSAOFramebuffer.h"
#include "../graphics/frameBuffers/MSAADepthNormalFramebuffer.h"


#include "../utils/fps.h"
#include "../utils/imGuiScreens.h"


namespace Lengine {

    using SSAOBlurFramebuffer = SSAOFramebuffer;


    enum class ViewportMode {
        first = 0,
        second,
        third,
        count 
    };

    class ViewportPanel {
    public:
        ViewportPanel(Camera3d& camera, const glm::i32vec2 resolution);
        void OnImGuiRender();
        void RenderFullscreen();

        // Access framebuffer for rendering the scene
        Framebuffer& GetFramebuffer() { return Framebuffer; }
        MSAAFramebuffer& GetMSAAFramebuffer() { return MSAAFramebuffer; }
        HDRFramebuffer& GetHDRFramebuffer() { return HDRFramebuffer; }
        MSAAHDRFramebuffer& GetMSAAHDRFramebuffer() { return MSAAHDRFramebuffer; }

        SSAOFramebuffer& GetSSAOFramebuffer() { return SSAOFramebuffer; }
        SSAOBlurFramebuffer& GetSSAOBlurFramebuffer() { return SSAOBlurFramebuffer; }
        DepthNormalFramebuffer& GetDepthNormalFramebuffer() { return depthNormalFramebuffer; }
        MSAADepthNormalFramebuffer& GetMSAADepthNormalFramebuffer() { return MSAADepthNormalFramebuffer; }



        void ClearFrame(const glm::vec4& clearColor);
        void ClearFramebuffers();

        // Check if the viewport has resized
        bool IsViewportFocused() const { return m_Focused; }
        bool IsViewportHovered() const { return m_Hovered; }

        ImVec2 GetViewportSize() const { return m_ViewportSize; }
        ImVec2 GetViewportPos() const { return m_ViewportPos; }

        ImVec2 getMousePosInViewport() const { return mouseInViewport; }
        ImVec2 getMousePosInImage() const { return mouseInImage; }

        bool fixCamera = false;
        bool viewportFullscreen = true;

        
    private:
        Camera3d& camera;
        float fullscreenAspectRatio = 16.0f / 9.0f;

        Framebuffer Framebuffer;
        MSAAFramebuffer MSAAFramebuffer;
        HDRFramebuffer HDRFramebuffer;
        MSAAHDRFramebuffer MSAAHDRFramebuffer;
        
        SSAOFramebuffer SSAOFramebuffer;
        SSAOBlurFramebuffer SSAOBlurFramebuffer;
        DepthNormalFramebuffer depthNormalFramebuffer;
        MSAADepthNormalFramebuffer MSAADepthNormalFramebuffer;


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
