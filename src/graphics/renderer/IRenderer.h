#pragma once

#include "../scene/Scene.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"

namespace Lengine {

    struct RenderContext {
        Scene* scene;

        glm::mat4 cameraView;
        glm::mat4 cameraProjection;
        glm::vec3 cameraPos;

        ShadowMap* shadowMap;
        ShadowCubeMap* shadowCubeMap;

        GLTexture irradianceMap;
        GLTexture prefilterMap;
        GLTexture brdfLUTMap;
        float envIntensity;
        glm::vec3 envTint;
        glm::mat3 envRotation;

        RenderSettings* settings;


    };

    enum class DebugView : int {
        Geometry = 0,   
        Albedo = 1,
        Normal = 2,
        Depth = 3
    };

   
    class IRenderer {
    public:
        static bool enableDebugView;
        static DebugView debugViewMode;
        virtual ~IRenderer() = default;

        virtual void Render(
            const RenderContext& ctx
        ) = 0;


    private:
       
    };
    
}

