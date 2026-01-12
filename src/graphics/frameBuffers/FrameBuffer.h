#pragma once
#include <GL/glew.h>

namespace Lengine {

    class Framebuffer {
    public:
        Framebuffer(float width, float height);
        ~Framebuffer();

        void Create();
        void Destroy();

        void Bind();
        void Unbind();

        void Resize(float width, float height);

        GLuint GetColorAttachment() const { return m_ColorAttachment; }
        GLuint GetID() const { return FBO; }

        float GetWidth() const { return width; }
        float GetHeight() const { return height; }

        void useTexture(GLuint texture, GLuint slot);

    private:
       

    private:
        GLuint FBO = 0;
        GLuint m_ColorAttachment = 0;
        GLuint depthBuffer = 0;

        float width = 0;
        float height = 0;
    };

}
