#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/renderer/FrameBuffer.h"
namespace Lengine {

    class MSAAFramebuffer {
    public:
        MSAAFramebuffer(uint32_t width, uint32_t height);
        ~MSAAFramebuffer();

        void Create();
        void Bind();
        void Unbind();
        void Resize(uint32_t width, uint32_t height);
        void ResolveTo(const Framebuffer& target);

        void setMSAASamples(const uint8_t& samples) { m_Samples = samples; }

    private:
        
        void Destroy();

    private:
        uint32_t m_Width = 0, m_Height = 0;
        uint32_t m_Samples = 0;

        GLuint m_FBO = 0;
        GLuint m_ColorAttachment = 0;
        GLuint m_DepthAttachment = 0;
    };

}
