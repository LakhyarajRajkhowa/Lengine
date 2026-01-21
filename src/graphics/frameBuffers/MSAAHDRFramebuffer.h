#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/frameBuffers/HDRFrameBuffer.h"
namespace Lengine {

    class MSAAHDRFramebuffer {
    public:
        MSAAHDRFramebuffer(const uint32_t width, const uint32_t height);
        ~MSAAHDRFramebuffer();

        void Create();
        void Destroy();

        void Bind();
        void Unbind();

        void Resize(const uint32_t width, const uint32_t height);
        void ResolveToHDR(const HDRFramebuffer& target);

        void SetMSAASamples(const uint32_t n) { samples = n; }
        const GLuint* GetColorBuffers() const { return colorBuffers; }

    private:
        
    private:
        uint32_t width = 0, height = 0;
        uint32_t samples = 0;

        GLuint FBO = 0;
        GLuint colorBuffers[2];
        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        GLuint depthBuffer = 0;
    };

}
