#include "DepthNormalFramebuffer.h"

using namespace Lengine;

void DepthNormalFramebuffer::Create(uint32_t w, uint32_t h)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Normal texture
    glGenTextures(1, &normalBuffer);
    glBindTexture(GL_TEXTURE_2D, normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        normalBuffer,
        0
    );

    // Depth texture
    glGenTextures(1, &depthBuffer);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_DEPTH_COMPONENT24,
        w, h, 0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthBuffer,
        0
    );

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthNormalFramebuffer::Destroy() {
    if (depthBuffer) {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
    if (normalBuffer) {
        glDeleteTextures(1, &normalBuffer);
        normalBuffer = 0;
    }
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
}

void DepthNormalFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}


void DepthNormalFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
