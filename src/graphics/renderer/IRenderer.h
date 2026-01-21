#pragma once

#include "../scene/Scene.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"

namespace Lengine {


    
   


    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void Render(
            Scene& scene,
            const EditorConfig& editorConfig,
            ShadowMap& shadowMap,
            ShadowCubeMap& shadowCubeMap,
            const GLTexture& irradianceMap
        ) = 0;
    };
    
}

