#pragma once

#include <cstdint>


#include "../graphics/geometry/FullScreenQuad.h"
#include "../graphics/renderer/PostProcess/PingPongFrameBuffer.h"

#include "../graphics/opengl/GLTexture.h"
#include "../graphics/opengl/GLSLProgram.h"

#include "../core/paths.h"
#include <imgui.h>


namespace Lengine {

    class PostProcessing
    {
    public:
        PostProcessing::PostProcessing(RenderSettings& sett):
            settings(sett)
        {

        }
        void initHDR(uint32_t width, uint32_t height);
        void renderToneMapping(const bool bloom, const float exposure);
        
        void renderBloom(GLuint colorBuffer, const float blurScale);
        const GLuint* getBloomTextures() { return pingpong.colorBuffer; }
        const GLuint getBloomColorBuffer() { return pingpong.colorBuffer[!horizontal]; }

    private:
        uint32_t width = 1280;
        uint32_t height = 720;
    private:
        void initToneMappingResources();
        void initBloom();

    private:
        RenderSettings& settings;
        GLSLProgram toneMapShader;
        FullscreenQuad fullscreenQuad;

        GLSLProgram blurShader;
        float bloomBlurScale = 1.0f;
        int blurPasses = 10;
        bool horizontal = false;
        PingPongFramebuffer pingpong;

    };

}

