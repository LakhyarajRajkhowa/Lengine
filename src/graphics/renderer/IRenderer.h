#pragma once

#include "../scene/Scene.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"

namespace Lengine {


    
   


    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void render(
            Scene& scene,
            const EditorConfig& editorConfig,
            ShadowMap& shadowMap,
            ShadowCubeMap& shadowCubeMap
        ) = 0;
    };
    
}

