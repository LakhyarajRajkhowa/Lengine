#include "Framebuffer.h"
#include <iostream>

namespace Lengine {

    Framebuffer::Framebuffer(float width, float height)
        : m_Width(width), m_Height(height)
    {
        Create();
    }

    Framebuffer::~Framebuffer() {
        Destroy();
    }

    void Framebuffer::Create() {
        // Delete old framebuffer if exists
        Destroy();

        // Create framebuffer
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // Create color texture
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, m_ColorAttachment, 0);

        // Create depth-stencil renderbuffer
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            m_Width, m_Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, m_DepthAttachment);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[Framebuffer] ERROR: Framebuffer is incomplete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Destroy() {
        if (m_DepthAttachment) {
            glDeleteRenderbuffers(1, &m_DepthAttachment);
            m_DepthAttachment = 0;
        }
        if (m_ColorAttachment) {
            glDeleteTextures(1, &m_ColorAttachment);
            m_ColorAttachment = 0;
        }
        if (m_FBO) {
            glDeleteFramebuffers(1, &m_FBO);
            m_FBO = 0;
        }
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void Framebuffer::useTexture(const GLuint texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(float width, float height) {
        if (width == 0 || height == 0)
            return;

        m_Width = width;
        m_Height = height;
        Create();
    }

}
