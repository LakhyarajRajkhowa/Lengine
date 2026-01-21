#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/frameBuffers/FrameBuffer.h"
namespace Lengine {

    class HDRFramebuffer {
    public:
        HDRFramebuffer(const uint32_t width, const uint32_t height);
        ~HDRFramebuffer();

        GLuint GetID() const { return FBO; }
        const GLuint* GetColorBuffers() const { return colorBuffers; }

        void Create();
        void Destroy();

        void Bind();
        void Unbind();
        void Resize(const uint32_t width, const uint32_t height);
        void ResolveTo(const Framebuffer& target);
        
        uint32_t GetWidth() const { return width; }
        uint32_t GetHeight() const { return height; };

    private:

    private:
        uint32_t width = 0, height = 0;

        GLuint FBO = 0;
        GLuint depthBuffer = 0;
        GLuint colorBuffers[2];
        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    };

}
