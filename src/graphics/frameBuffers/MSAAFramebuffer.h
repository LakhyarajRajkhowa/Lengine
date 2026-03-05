#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/framebuffers/FrameBuffer.h"
namespace Lengine {

    class MSAAFramebuffer {
    public:
        MSAAFramebuffer(const uint32_t width, const uint32_t height);
        ~MSAAFramebuffer();

        void Create();
        void Destroy();

        void Bind();
        void Unbind();

        void Resize(const uint32_t width, const uint32_t height);
        void ResolveTo(const LDRFramebuffer& target);

        void SetMSAASamples(const uint32_t s) { samples = s; }

    private:
        

    private:
        uint32_t width = 0, height = 0;
        uint32_t samples = 4;

        GLuint FBO = 0;
        GLuint colorBuffer = 0;
        GLuint depthBuffer = 0;
    };

}
