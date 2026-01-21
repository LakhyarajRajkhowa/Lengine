#include "MSAAFramebuffer.h"
#include <iostream>

namespace Lengine {

    MSAAFramebuffer::MSAAFramebuffer(const uint32_t width, const uint32_t height)
        : width(width), height(height)
    {
        
    }

    MSAAFramebuffer::~MSAAFramebuffer() {
        Destroy();
    }

    void MSAAFramebuffer::Create() {

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Color attachment (multisampled)
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffer);
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            samples,
            GL_RGBA8,
            width,
            height,
            GL_TRUE
        );

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D_MULTISAMPLE,
            colorBuffer,
            0
        );

        // Depth + stencil
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorageMultisample(
            GL_RENDERBUFFER,
            samples,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            depthBuffer
        );

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[MSAAFramebuffer] ERROR: Framebuffer incomplete!\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAAFramebuffer::Destroy() {
        if (depthBuffer) glDeleteRenderbuffers(1, &depthBuffer);
        if (colorBuffer) {
            glDeleteTextures(1, &colorBuffer);
            colorBuffer = 0;
        } 
       
        if (FBO) glDeleteFramebuffers(1, &FBO);

        depthBuffer = colorBuffer = FBO = 0;
    }

    void MSAAFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void MSAAFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAAFramebuffer::Resize(const uint32_t width, const uint32_t height) {
        if (width == 0 || height == 0) return;
        this->width = width;
        this->height = height;
        Create();
    }



    void MSAAFramebuffer::ResolveTo(const Framebuffer& target) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetID());

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, target.GetWidth(), target.GetHeight(),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}
