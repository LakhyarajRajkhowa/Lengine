#include "HDRFramebuffer.h"
#include <iostream>

namespace Lengine {

    HDRFramebuffer::HDRFramebuffer(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) 
    {
        Create();
    }

    HDRFramebuffer::~HDRFramebuffer() {
        Destroy();
    }

    void HDRFramebuffer::Create() {
        Destroy();

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // HDR color buffer (resolved target)
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA16F,        // HDR
            m_Width,
            m_Height,
            0,
            GL_RGBA,
            GL_FLOAT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenRenderbuffers(1, &m_DepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
        // attach buffers
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRBO);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void HDRFramebuffer::Destroy() {
        if (m_DepthRBO) glDeleteRenderbuffers(1, &m_DepthRBO);
        if (m_ColorAttachment) glDeleteTextures(1, &m_ColorAttachment);
        if (m_FBO) glDeleteFramebuffers(1, &m_FBO);

        m_DepthRBO = m_ColorAttachment = m_FBO = 0;
    }

    void HDRFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void HDRFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void HDRFramebuffer::Resize(uint32_t width, uint32_t height) {
        if (width == 0 || height == 0) return;
        m_Width = width;
        m_Height = height;
        Create();
    }

    void HDRFramebuffer::ResolveTo(const Framebuffer& target) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetID());

        glBlitFramebuffer(
            0, 0, m_Width, m_Height,
            0, 0, m_Width, m_Height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
