#pragma once
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/camera/Camera3d.h"
#include "../resources/TextureCache.h"

#include "../resources/AssetManager.h"

namespace Lengine {
    class Renderer {
    public:
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 lightPos;
        float ambientStrength = 0.05f;
        float specularStrength = 3.5f;
        bool changeColor = false;

        void renderScene(const Scene& scene, Camera3d& camera, AssetManager& assetManaegr);

        std::unordered_set<std::string> loadedEntity;
    };
}
