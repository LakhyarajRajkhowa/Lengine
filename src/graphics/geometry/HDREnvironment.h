#pragma once

#include "../resources/ImageLoader.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../graphics/geometry/FullScreenQuad.h"
#include <string>

namespace Lengine {

    class HDREnvironment {
    public:
        void Init(const uint32_t texRes);
        void CompileShaders();
        void Render(const glm::mat4& view, const glm::mat4& projection);

        const GLTexture GetHDRTexture() { return hdrTexture; }
        void SetHDRTexture(const GLTexture& tex) { hdrTexture = tex; updateTex(); }

        const GLTexture GetIrradianceMap() { return irradianceMap; }
        const GLTexture GetPrefilterMap() { return prefilterMap; }
        const GLTexture GetbrdfLUTMap() { return brdfLUTTexture; }

    private:


    private:

        GLSLProgram equirectToCubeShader;
        GLSLProgram backgroundShader;
        GLSLProgram irradianceShader;
        GLSLProgram prefilterShader;
        GLSLProgram brdfShader;

        GLTexture hdrTexture;      // raw HDR 2D
        GLTexture envCubemap;        // converted environment cube
        GLTexture irradianceMap;     // diffuse convolution cube
        GLTexture prefilterMap;
        GLTexture brdfLUTTexture;

        FullscreenQuad quad;

        unsigned int captureFBO = 0;
        unsigned int captureRBO = 0;

        glm::mat4 captureProjection;
        std::vector<glm::mat4> captureViews;
        uint32_t texSize = 1024;

        void updateTex();

    private:
        unsigned int cubeVAO = 0;
        unsigned int cubeVBO = 0;

        void renderCube();

    public:
        float envRotationAngle = 0.0f;
        glm::mat3 rot = glm::mat3(1.0f);
        float envIntensity = 1.0f;
        glm::vec3 envTint = glm::vec3(1.0f);
    };
}
