#include "MSAAFramebuffer.h"
#include <iostream>

namespace Lengine {

    MSAAFramebuffer::MSAAFramebuffer(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        
    }

    MSAAFramebuffer::~MSAAFramebuffer() {
        Destroy();
    }

    void MSAAFramebuffer::Create() {
        Destroy();

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // Color attachment (multisampled)
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            m_Samples,
            GL_RGBA16F,
            m_Width,
            m_Height,
            GL_TRUE
        );

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE,
            m_ColorAttachment,
            0
        );

        // Depth + stencil
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER,
            m_Samples,
            GL_DEPTH24_STENCIL8,
            m_Width,
            m_Height
        );

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            m_DepthAttachment
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[MSAAFramebuffer] ERROR: Framebuffer incomplete!\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAAFramebuffer::Destroy() {
        if (m_DepthAttachment) glDeleteRenderbuffers(1, &m_DepthAttachment);
        if (m_ColorAttachment) glDeleteTextures(1, &m_ColorAttachment);
        if (m_FBO) glDeleteFramebuffers(1, &m_FBO);

        m_DepthAttachment = m_ColorAttachment = m_FBO = 0;
    }

    void MSAAFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void MSAAFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAAFramebuffer::Resize(uint32_t width, uint32_t height) {
        if (width == 0 || height == 0) return;
        m_Width = width;
        m_Height = height;
        Create();
    }

    void MSAAFramebuffer::ResolveTo(const HDRFramebuffer& target) {
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

    void MSAAFramebuffer::ResolveTo(const Framebuffer& target) {
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
