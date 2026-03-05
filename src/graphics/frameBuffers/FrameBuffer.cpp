#include "Framebuffer.h"
#include <iostream>


using namespace Lengine;

LDRFramebuffer::LDRFramebuffer(const uint32_t w, const uint32_t h)
    : width(w), height(h)
{
    Create();
}

LDRFramebuffer::~LDRFramebuffer() {
    Destroy();
}

void LDRFramebuffer::Create() {

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

void LDRFramebuffer::Destroy() {
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

void LDRFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}


void LDRFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void LDRFramebuffer::UseTexture(const GLuint texture, const GLuint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}


void LDRFramebuffer::Resize(const uint32_t width, const uint32_t height) {
    if (width == 0 || height == 0)
        return;

    this->width = width;
    this->height = height;
    Create();
}





static bool IsMultisampled(uint32_t samples)
{
    return samples > 1;
}

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_Spec(spec)
{
    Invalidate();
}

Framebuffer::~Framebuffer()
{
    Destroy();
}

void Framebuffer::Invalidate()
{
    if (m_FBO)
        Destroy();

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    bool multisample = IsMultisampled(m_Spec.samples);

    // ---------- Color Attachments ----------
    m_ColorAttachments.resize(m_Spec.colorAttachmentCount);
    glGenTextures(m_Spec.colorAttachmentCount, m_ColorAttachments.data());

    for (uint32_t i = 0; i < m_Spec.colorAttachmentCount; i++)
    {
        if (multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i]);
            glTexImage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE,
                m_Spec.samples,
                m_Spec.colorFormat,
                m_Spec.width,
                m_Spec.height,
                GL_TRUE
            );

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D_MULTISAMPLE,
                m_ColorAttachments[i],
                0
            );
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                m_Spec.colorFormat,
                m_Spec.width,
                m_Spec.height,
                0,
                GL_RGBA,
                GL_FLOAT,
                nullptr
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D,
                m_ColorAttachments[i],
                0
            );
        }
    }

    // Draw buffers
    std::vector<GLenum> buffers;
    for (uint32_t i = 0; i < m_Spec.colorAttachmentCount; i++)
        buffers.push_back(GL_COLOR_ATTACHMENT0 + i);

    glDrawBuffers(buffers.size(), buffers.data());

    // ---------- Depth ----------
    if (m_Spec.useDepth)
    {
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);

        if (multisample)
        {
            glRenderbufferStorageMultisample(
                GL_RENDERBUFFER,
                m_Spec.samples,
                GL_DEPTH24_STENCIL8,
                m_Spec.width,
                m_Spec.height
            );
        }
        else
        {
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                GL_DEPTH24_STENCIL8,
                m_Spec.width,
                m_Spec.height
            );
        }

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            m_DepthAttachment
        );
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "[Framebuffer] ERROR: Incomplete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Destroy()
{
    if (m_DepthAttachment)
    {
        glDeleteRenderbuffers(1, &m_DepthAttachment);
        m_DepthAttachment = 0;
    }

    if (!m_ColorAttachments.empty())
    {
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        m_ColorAttachments.clear();
    }

    if (m_FBO)
    {
        glDeleteFramebuffers(1, &m_FBO);
        m_FBO = 0;
    }
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
        return;

    m_Spec.width = width;
    m_Spec.height = height;
    Invalidate();
}

uint32_t Framebuffer::GetColorAttachment(uint32_t index) const
{
    return m_ColorAttachments[index];
}


