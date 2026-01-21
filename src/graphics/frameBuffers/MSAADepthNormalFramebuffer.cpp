#include "MSAADepthNormalFramebuffer.h"

using namespace Lengine;

void MSAADepthNormalFramebuffer::Create(uint32_t w, uint32_t h, int samples)
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // Normal (MSAA)
    glGenTextures(1, &m_NormalMS);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_NormalMS);
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        samples,
        GL_RGB16F,
        w, h,
        GL_TRUE
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        m_NormalMS,
        0
    );

    // Depth
    glGenRenderbuffers(1, &m_DepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        samples,
        GL_DEPTH_COMPONENT24,
        w, h
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        m_DepthRBO
    );

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void MSAADepthNormalFramebuffer::Destroy() {
   
}

void MSAADepthNormalFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}


void MSAADepthNormalFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
