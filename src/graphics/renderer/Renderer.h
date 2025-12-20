#pragma once
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/camera/Camera3d.h"
#include "../graphics/lightning/Light.h"
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
        std::vector<Light> lights;
        void renderScene(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);

       // void collectRenderData(Scene& scene, Camera3d& camera, AssetManager& assetManager);
       // void flushBatches(Scene& scene,
       //     Camera3d& camera,
       //     AssetManager& assetManager);

       // void renderSceneECS(Scene& scene, Camera3d& camera, AssetManager& assetManaegr);
       // RenderBatcher batcher;
    private:
        ResolvedMaterial resolveMaterial(
            const Material& baseMaterial,
            const MaterialInstance& inst
        );
        void bindCameraUniforms(
            GLSLProgram& shader,
            const glm::mat4& model,
            Camera3d& camera
            );
        void bindLightUniforms(
            GLSLProgram& shader,
            const Light& light,
            int index
        );
        void bindMaterialUniforms(
            GLSLProgram& shader,
            const ResolvedMaterial& mat
        );
        void bindTexture(
            GLSLProgram& shader,
            AssetManager& assetManager,
            const UUID& texID,
            const char* hasUniform,
            const char* samplerUniform,
            GLenum textureUnit
        );
        void drawSubMesh(
            SubMesh& sm,
            GLSLProgram& shader,
            bool isHovered
        );
        void drawSubMeshGroup(
            Mesh& mesh,
            const std::vector<uint32_t>& subMeshIDs,
            GLSLProgram& shader,
            const std::unordered_set<uint32_t>& hoveredSubMeshes
        );
        void collectLights( const std::vector<std::unique_ptr<Entity>>& entities);


    };
}
