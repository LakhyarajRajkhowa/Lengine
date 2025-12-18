#pragma once
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/camera/Camera3d.h"
#include "../resources/TextureCache.h"

#include "../resources/AssetManager.h"

namespace Lengine {

    /*
    struct RenderCommand {
        uint32_t entity;
        glm::mat4 model;
    };

    struct RenderKey {
        GLSLProgram* shader;
        UUID materialID;
        UUID meshID;
        uint32_t subMeshIndex;

        bool operator==(const RenderKey& o) const {
            return shader == o.shader &&
                materialID == o.materialID &&
                meshID == o.meshID &&
                subMeshIndex == o.subMeshIndex;
        }
    };
    struct RenderKeyHash {
        size_t operator()(const RenderKey& k) const {
            size_t h = 0;
            h ^= std::hash<void*>()(k.shader);
            h ^= std::hash<UUID>()(k.materialID) << 1;
            h ^= std::hash<UUID>()(k.meshID) << 2;
            h ^= std::hash<uint32_t>()(k.subMeshIndex) << 3;
            return h;
        }
    };

    class RenderBatcher {
    public:
        using BatchMap =
            std::unordered_map<RenderKey,
            std::vector<RenderCommand>,
            RenderKeyHash>;

        void submit(const RenderKey& key, const RenderCommand& cmd) {
            batches[key].push_back(cmd);
        }

        void clear() {
            batches.clear();
        }

        BatchMap batches;
    };
    */

    enum class LightType {
        Directional = 0,
        Point = 1,
        Spotlight = 2
    };

    struct Light {
        LightType type = LightType::Directional;
        glm::vec3 position = glm::vec3(10.0f, 10.0f, 10.0f);
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);

        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(.5f, 0.5f, 0.5f);
    };
   

    class Renderer {
    public:
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 lightPos = glm::vec3(1000.0f, 1000.0f, 1000.0f);
        
        bool changeColor = false;

        void applyMaterialUniforms(GLSLProgram* shader,
            UUID materialID,
            AssetManager& assetManager);
        std::unordered_set<std::string> loadedEntity;
        void renderScene(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);

        Light light;
        

       // void collectRenderData(Scene& scene, Camera3d& camera, AssetManager& assetManager);
       // void flushBatches(Scene& scene,
       //     Camera3d& camera,
       //     AssetManager& assetManager);

       // void renderSceneECS(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);
       // RenderBatcher batcher;
    };
}
