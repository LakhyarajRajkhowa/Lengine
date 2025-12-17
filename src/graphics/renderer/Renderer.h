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

   

    class Renderer {
    public:
        glm::vec3 lightColor = glm::vec3(1.0f);
        glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);
        
        bool changeColor = false;

        void applyMaterialUniforms(GLSLProgram* shader,
            UUID materialID,
            AssetManager& assetManager);
        std::unordered_set<std::string> loadedEntity;
        void renderScene(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);


       // void collectRenderData(Scene& scene, Camera3d& camera, AssetManager& assetManager);
       // void flushBatches(Scene& scene,
       //     Camera3d& camera,
       //     AssetManager& assetManager);

       // void renderSceneECS(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);
       // RenderBatcher batcher;
    };
}
