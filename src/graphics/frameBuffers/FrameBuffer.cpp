#include "Framebuffer.h"
#include <iostream>

namespace Lengine {

    Framebuffer::Framebuffer(const uint32_t w, const uint32_t h)
        : width(w), height(h)
    {
        Create();
    }

    Framebuffer::~Framebuffer() {
        Destroy();
    }

    void Framebuffer::Create() {

        // Create framebuffer
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Create color texture
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, colorBuffer, 0);

        // Create depth-stencil renderbuffer
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, depthBuffer);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[Framebuffer] ERROR: Framebuffer is incomplete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Destroy() {
        if (depthBuffer) {
            glDeleteRenderbuffers(1, &depthBuffer);
            depthBuffer = 0;
        }
        if (colorBuffer) {
            glDeleteTextures(1, &colorBuffer);
            colorBuffer = 0;
        }
        if (FBO) {
            glDeleteFramebuffers(1, &FBO);
            FBO = 0;
        }
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }


    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Framebuffer::UseTexture(const GLuint texture, const GLuint slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }


    void Framebuffer::Resize(const uint32_t width, const uint32_t height) {
        if (width == 0 || height == 0)
            return;

        this->width = width;
        this->height = height;
        Create();
    }

}
