#pragma once

#include <cstdint>


#include "../graphics/geometry/FullScreenQuad.h"
#include "../graphics/renderer/PostProcess/PingPongFrameBuffer.h"

#include "../graphics/opengl/GLTexture.h"
#include "../graphics/opengl/GLSLProgram.h"

#include "../core/paths.h"


namespace Lengine {


    class PostProcessing
    {
    public:

        void InitHDRShaders();

        void InitToneMappingResources();
        void InitBloom(uint32_t width, uint32_t height);
        void renderToneMapping(const bool bloom, const float exposure);
        void renderBloomShader();

        
        void renderBloom(GLuint colorBuffer, const float blurScale);
        const GLuint* getBloomTextures() { return pingpong.colorBuffer; }
        const GLuint getBloomColorBuffer() { return pingpong.colorBuffer[!horizontal]; }

    private:


    private:
        GLSLProgram hdrToneMappingShader;
        GLSLProgram hdrBloomShader;
        FullscreenQuad fullscreenQuad;

        GLSLProgram blurShader;
        float bloomBlurScale = 1.0f;
        int blurPasses = 10;
        bool horizontal = false;
        PingPongFramebuffer pingpong;

    };

}

