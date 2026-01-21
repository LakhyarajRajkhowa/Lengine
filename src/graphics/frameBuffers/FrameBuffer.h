#pragma once
#include <cstdint>
#include <GL/glew.h>
#include "../graphics/opengl/GLTexture.h"

namespace Lengine {

    class Framebuffer {
    public:
        Framebuffer(const uint32_t w, const uint32_t h);
        ~Framebuffer();

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

}
