#pragma once
#include <../graphics/frameBuffers/DepthNormalFramebuffer.h>

namespace Lengine {
    class MSAADepthNormalFramebuffer {
    public:
        void Create(uint32_t width, uint32_t height, int samples);
        void Destroy();

        void Bind();
        void Unbind();

        void ResolveTo(DepthNormalFramebuffer& target);

        GLuint GetNormalAttachment() const { return m_NormalMS; }

    private:
        GLuint m_FBO = 0;
        GLuint m_NormalMS = 0;
        GLuint m_DepthRBO = 0;
    };
}

