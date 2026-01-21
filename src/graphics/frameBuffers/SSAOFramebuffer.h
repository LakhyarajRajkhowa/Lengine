#pragma once
#include <cstdint>

#include "../graphics/opengl/GLSLProgram.h"

namespace Lengine {
    class SSAOFramebuffer {
    public:
        void Create(uint32_t w, uint32_t h);
        void Destroy();

        void Bind();
        void Unbind();

        GLuint GetColor() const { return m_AO; }

    private:
        GLuint m_FBO = 0;
        GLuint m_AO = 0;
    };

}