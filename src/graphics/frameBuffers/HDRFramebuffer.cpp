#include "HDRFramebuffer.h"
#include <iostream>

namespace Lengine {

    HDRFramebuffer::HDRFramebuffer(const uint32_t width, const uint32_t height)
        : width(width), height(height) 
    {
        Create();
    }

    HDRFramebuffer::~HDRFramebuffer() {
        Destroy();
    }

    void HDRFramebuffer::Create() {

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // HDR color buffer (resolved target)
        glGenTextures(2, colorBuffers);

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA16F,        // HDR
                width,
                height,
                0,
                GL_RGBA,
                GL_FLOAT,
                nullptr
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // attach texture to framebuffer
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
            );
        }

        glDrawBuffers(2, attachments);

        // Create depth-stencil renderbuffer
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void HDRFramebuffer::Destroy() {
        if (depthBuffer) {
            glDeleteRenderbuffers(1, &depthBuffer);
            depthBuffer = 0;
        } 

        for (uint8_t i = 0; i < 2; i++) {
            if (colorBuffers[i]) {
                glDeleteTextures(1, &colorBuffers[i]);
                colorBuffers[i] = 0;
            }
        }

        if (FBO) {
            glDeleteFramebuffers(1, &FBO);
            FBO = 0;
        }

    }

    void HDRFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void HDRFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void HDRFramebuffer::Resize(const uint32_t width, const uint32_t height) {
        if (width == 0 || height == 0) return;
        this->width = width;
        this->height = height;
        Create();
    }

    void HDRFramebuffer::ResolveTo(const Framebuffer& target) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetID());

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
