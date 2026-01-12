#pragma once

#include "../editor/EditorLayer.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/camera/Camera3d.h"
#include "../graphics/lightning/Light.h"
#include "../graphics/renderer/IRenderer.h"
#include "../graphics/shadowMaps/shadowMap.h"
#include "../graphics/shadowMaps/shadowCubeMap.h"
#include "../resources/TextureCache.h"

#include "../resources/AssetManager.h"

namespace Lengine {

   
    struct RenderFlags {
        bool entitySelected = false;
        bool entityDragged = false;
    };


    class ForwardRenderer : public IRenderer {
    public:
        ForwardRenderer(
            Camera3d& cam,
            AssetManager& assetmgr

        ) :
            camera(cam),
            assetManager(assetmgr)
        {
        }

        void render(
            Scene& scene,
            const EditorConfig& editorConfig,
            ShadowMap& shadowMap,
            ShadowCubeMap& shadowCubeMap
        ) override
        {
            renderScene(scene, editorConfig, shadowMap, shadowCubeMap);
        }

        

    private:
        Camera3d& camera;
        AssetManager& assetManager;

        ResolvedMaterial resolveMaterial(
            const Material& baseMaterial,
            const MaterialInstance& inst
        );
        void renderScene(
            Scene& activeScene,
            const EditorConfig& editorConfig,
            ShadowMap& shadowMap,
            ShadowCubeMap& shadowCubeMap
        );

        void bindShadowMapUniforms(
            GLSLProgram& shader,
            ShadowMap& shadowMap,
            Light& mainDirectionalLight
        );
        void bindPointShadowUniforms(
            GLSLProgram& shader,
            ShadowCubeMap& shadowCubeMap,
            Light& light
        );

        void bindCameraUniforms(
            GLSLProgram& shader,
            const glm::mat4& model,
            Camera3d& camera
        );
        void bindLightUniforms(
            GLSLProgram& shader,
            const Light& light,
            const glm::vec3& sceneAmbient,
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
            const bool  use,
            const char* hasUniform,
            const char* samplerUniform,
            GLenum textureUnit
        );
        void bindEditorUniforms(
            GLSLProgram& shader,
            const RenderFlags& mat,
            const EditorConfig& editorConfig
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
        void collectLights(std::vector<Light>& lights, const std::vector<std::unique_ptr<Entity>>& entities);

        GLSLProgram outlineShader;
        void ForwardRenderer::drawMeshAllSubMeshes(
            Mesh& mesh,
            GLSLProgram& shader
        );

        GLSLProgram* defaultShader;
        GLSLProgram* lightSourceShader;
    };
}

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
