#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/frameBuffers/FrameBuffer.h"
namespace Lengine {

    class HDRFramebuffer {
    public:
        HDRFramebuffer(uint32_t width, uint32_t height);
        ~HDRFramebuffer();

        GLuint GetID() const { return FBO; }
        const GLuint* GetColorBuffers() const { return colorBuffers; }

        void Create();
        void Destroy();

        void Bind();
        void Unbind();
        void Resize(uint32_t width, uint32_t height);
        void ResolveTo(const Framebuffer& target);
        
        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; };

    private:

    private:
        uint32_t width = 0, height = 0;

        GLuint FBO = 0;
        GLuint depthRBO = 0;
        GLuint colorBuffers[2];
        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        GLuint depthBuffer = 0;
    };

}
