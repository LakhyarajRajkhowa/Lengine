#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/renderer/FrameBuffer.h"
namespace Lengine {

    class HDRFramebuffer {
    public:
        HDRFramebuffer(uint32_t width, uint32_t height);
        ~HDRFramebuffer();

        GLuint GetID() const { return m_FBO; }
        GLuint GetColorTexture() const { return m_ColorAttachment; }

        void Create();
        void Bind();
        void Unbind();
        void Resize(uint32_t width, uint32_t height);
        void ResolveTo(const Framebuffer& target);
         

    private:

        void Destroy();

    private:
        uint32_t m_Width = 0, m_Height = 0;

        GLuint m_FBO = 0;
        GLuint m_DepthRBO = 0;
        GLuint m_ColorAttachment = 0;
        GLuint m_DepthAttachment = 0;
    };

}
