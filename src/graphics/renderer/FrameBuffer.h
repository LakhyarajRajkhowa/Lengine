#pragma once
#include <GL/glew.h>

namespace Lengine {

    class Framebuffer {
    public:
        Framebuffer(float width, float height);
        ~Framebuffer();

        void Bind();
        void Unbind();

        void Resize(float width, float height);

        GLuint GetColorAttachment() const { return m_ColorAttachment; }
        GLuint GetID() const { return m_FBO; }

        float GetWidth() const { return m_Width; }
        float GetHeight() const { return m_Height; }

        void useTexture(const GLuint Texture);

    private:
        void Create();
        void Destroy();

    private:
        GLuint m_FBO = 0;
        GLuint m_ColorAttachment = 0;
        GLuint m_DepthAttachment = 0;

        float m_Width = 0;
        float m_Height = 0;
    };

}
