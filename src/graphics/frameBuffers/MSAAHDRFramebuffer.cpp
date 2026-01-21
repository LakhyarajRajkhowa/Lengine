#include "MSAAHDRFramebuffer.h"
#include <iostream>

namespace Lengine {

    MSAAHDRFramebuffer::MSAAHDRFramebuffer(const uint32_t width, const uint32_t height)
        : width(width), height(height)
    {
        
    }

    MSAAHDRFramebuffer::~MSAAHDRFramebuffer() {
        Destroy();
    }

    void MSAAHDRFramebuffer::Create() {

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Color attachment (multisampled)
        glGenTextures(2, colorBuffers);
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i]);
            glTexImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE,
                samples,
                GL_RGBA16F,
                width,
                height,
                GL_TRUE
            );

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D_MULTISAMPLE,
               colorBuffers[i],
                0
            );
        }

        glDrawBuffers(2, attachments);


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

    void MSAAHDRFramebuffer::Destroy() {
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

    void MSAAHDRFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void MSAAHDRFramebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MSAAHDRFramebuffer::Resize(const uint32_t width, const uint32_t height) {
        if (width == 0 || height == 0) return;
        this->width = width;
        this->height = height;
        Create();
    }

    void MSAAHDRFramebuffer::ResolveToHDR(const HDRFramebuffer& target)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetID());

        const uint32_t attachmentCount = 2; // color + bright

        for (uint32_t i = 0; i < attachmentCount; i++)
        {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

            glBlitFramebuffer(
                0, 0, width,height,
                0, 0, target.GetWidth(), target.GetHeight(),
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST
            );
        }

        // restore default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glReadBuffer(GL_BACK);
        glDrawBuffer(GL_BACK);
    }

}
