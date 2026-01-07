#pragma once

#include <cstdint>

#include "../graphics/opengl/GLTexture.h"
#include "../graphics/opengl/GLSLProgram.h"

#include "../core/paths.h"
#include <imgui.h>
class FullscreenQuad {
public:
    void init();
    void draw();
private:
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
};

namespace Lengine {

    class PostProcessing
    {
    public:
        PostProcessing();
        void initHDR();
        void renderToneMapping();
        void drawExposureEditor();
        
        void setExposure(float exposure);
        float getExposure() const;

    private:
        void initToneMappingResources();

    private:
        GLSLProgram m_ToneMapShader;
        FullscreenQuad m_FullscreenQuad;

        float m_Exposure = 1.0f;
    };

}

