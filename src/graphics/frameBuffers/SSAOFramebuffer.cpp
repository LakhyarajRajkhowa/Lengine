#include "SSAOFramebuffer.h"

using namespace Lengine;

void SSAOFramebuffer::Create(uint32_t w, uint32_t h)
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_AO);
    glBindTexture(GL_TEXTURE_2D, m_AO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_AO,
        0
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOFramebuffer::Destroy() {

}

void SSAOFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}


void SSAOFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}