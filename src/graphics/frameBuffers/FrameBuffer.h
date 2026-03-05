#pragma once
#include <cstdint>
#include <GL/glew.h>
#include "../graphics/opengl/GLTexture.h"

namespace Lengine {

    class LDRFramebuffer {
    public:
        LDRFramebuffer(const uint32_t w, const uint32_t h);
        ~LDRFramebuffer();

        void Create();
        void Destroy();

        void Bind();
        void Unbind();

        void Resize(const uint32_t width, const uint32_t height);

        GLuint GetColorBuffer() const { return colorBuffer; }
        GLuint GetID() const { return FBO; }

        uint32_t GetWidth() const { return width; }
        uint32_t GetHeight() const { return height; }

        void UseTexture(const GLuint texture, const GLuint slot);

    private:
       

    private:
        GLuint FBO = 0;
        GLuint colorBuffer = 0;
        GLuint depthBuffer = 0;

        uint32_t width = 0;
        uint32_t height = 0;
    };



    struct FramebufferSpecification
    {
        uint32_t width = 1280;
        uint32_t height = 720;

        uint32_t samples = 1;            // 1 = no MSAA
        GLenum   colorFormat = GL_RGBA8; // GL_RGBA8 or GL_RGBA16F

        uint32_t colorAttachmentCount = 1;
        bool     useDepth = true;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        void Bind() const;
        void Unbind() const;

        void Resize(uint32_t width, uint32_t height);
        void Destroy();

        uint32_t GetID() const { return m_FBO; }
        uint32_t GetColorAttachment(uint32_t index = 0) const;
        uint32_t GetWidth() const { return m_Spec.width; }
        uint32_t GetHeight() const { return m_Spec.height; }
        uint32_t GetSamples() const { return m_Spec.samples; }

        const FramebufferSpecification& GetSpecification() const { return m_Spec; }

        void Invalidate(); // Create internally

    private:
        uint32_t m_FBO = 0;
        uint32_t m_DepthAttachment = 0;
        std::vector<uint32_t> m_ColorAttachments;

        FramebufferSpecification m_Spec;
    };


}
