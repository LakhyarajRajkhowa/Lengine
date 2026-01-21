#pragma once
#include <cstdint>

#include "../graphics/opengl/GLSLProgram.h"

namespace Lengine{
    class DepthNormalFramebuffer {
    public:
        void Create(uint32_t width, uint32_t height);
        void Destroy();

        void Bind();
        void Unbind();

        GLuint GetNormal() const { return normalBuffer; }
        GLuint GetDepth()  const { return depthBuffer; }

    private:
        GLuint FBO = 0;
        GLuint normalBuffer = 0;
        GLuint depthBuffer = 0;
    };
}

