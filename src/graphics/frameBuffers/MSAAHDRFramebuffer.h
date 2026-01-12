#pragma once
#include <GL/glew.h>
#include <cstdint>
#include "../graphics/frameBuffers/HDRFrameBuffer.h"
namespace Lengine {

    class MSAAHDRFramebuffer {
    public:
        MSAAHDRFramebuffer(uint32_t width, uint32_t height);
        ~MSAAHDRFramebuffer();

        void Create();
        void Destroy();

        void Bind();
        void Unbind();

        void Resize(uint32_t width, uint32_t height);
        void ResolveToHDR(const HDRFramebuffer& target);

        void setMSAASamples(const uint8_t n) { samples = n; }
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
