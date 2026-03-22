#pragma once

#include "../resources/TextureCache.h"

#include "../scene/Entity.h"
#include "../scene/components/MeshRendererStorage.h"
#include "../scene/components/MeshFilterStorage.h"
#include "../scene/components/LightStorage.h"
#include "../scene/components/TransformStorage.h"
#include "../scene/components/HierarchyStorage.h"
#include "../scene/components/NameTagComponentStorage.h"
#include "../scene/components/SkeletonComponentStorage.h"
#include "../scene/components/AnimationComponentStorage.h"

#include "../scene/TransformSystem.h"
#include "../assets/MaterialRegistry.h"
namespace Lengine {
    class Scene {
    public:
        
        Scene(const std::string& n, UUID sID)
            : name(n), sceneID(sID) 
        {
        }
  
        Entity* createEntity(
            const std::string& name,
            UUID entityID = UUID()
        );

        Entity* addEntity(std::unique_ptr<Entity> entity, const UUID originalEntityId);
       
        const  Entity* getEntityByID(const UUID& id) const;
        Entity* getEntityByID(const UUID& UUID);


        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
            return entities;
        }

        const std::vector<UUID>& getRootEntities() const { return rootEntities; }
        std::vector<UUID>& Scene::getRootEntities() {
            return rootEntities;
        }

        bool Scene::IsRootEntity(const UUID& id)
        {
            const auto& roots = getRootEntities();

            return std::find(roots.begin(), roots.end(), id) != roots.end();
        }

        const UUID& GetMainDirectionalLight() {
            Light light;
            bool hasDirectionalLight = false;

            for (auto& l : lights.GetAll()) {
                if (l.second.type == LightType::Directional) {
                    light.id = l.first;
                    light.outerAngle = l.second.outerAngle;
                    hasDirectionalLight = true;
                    break;
                }
            }
            if (!hasDirectionalLight) return UUID::Null;

            if (!transforms.Has(light.id)) return UUID::Null;



            return light.id;
        }

        UUID GetMainDirectionalLight() const {
            Light light;
            bool hasDirectionalLight = false;

            for (auto& l : lights.GetAll()) {
                if (l.second.type == LightType::Directional) {
                    light.id = l.first;
                    light.outerAngle = l.second.outerAngle;
                    hasDirectionalLight = true;


                    break;
                }
            }

            if (!hasDirectionalLight) return UUID::Null;

            if (!transforms.Has(light.id)) return UUID::Null;


            return light.id;
        }


 
        const std::string& getName() const { return name; }
        void rename(const std::string newName) { name = newName; }
        std::string getName() { return name; }
        UUID getUUID() const { return sceneID; }

        bool HasChildren(UUID entityID) const;
        const std::vector<UUID>& GetChildren(UUID entityID) const;

        void SetParent(UUID child, UUID parent);
        void MakeOrphan(UUID child);
        void RemoveEntity(const UUID);
        void RemoveEntityRecursive(UUID id);


        
        const MeshRendererStorage& MeshRenderers() const {
            return meshRenderers;
        }

        MeshRendererStorage& MeshRenderers() {
            return meshRenderers;
        }

        const MeshFilterStorage& MeshFilters() const {
            return meshFilters;
        }

        MeshFilterStorage& MeshFilters() {
            return meshFilters;
        }

        const LightStorage& Lights() const {
            return lights;
        }

        LightStorage& Lights() {
            return lights;
        }

        const TransformStorage& Transforms() const {
            return transforms;
        }

        TransformStorage& Transforms() {
            return transforms;
        }

        const HierarchyStorage& Hierarchys() const {
            return hierarchys;
        }

        HierarchyStorage& Hierarchys() {
            return hierarchys;
        }

        const NameTagComponentStorage& NameTags() const {
            return nameTags;
        }

        NameTagComponentStorage& NameTags() {
            return nameTags;
        }

        const SkeletonComponentStorage& Skeletons() const {
            return skeletons;
        }

         SkeletonComponentStorage& Skeletons()  {
            return skeletons;
        }

        const AnimationComponentStorage& Animations() const {
            return animations;
        }

         AnimationComponentStorage& Animations()  {
            return animations;
        }


        void UpdateWorldTransformRecursive(
            UUID entityID,
            const glm::mat4& parentWorld,
            bool parentDirty
        );
        void UpdateWorldTransforms();

        void Update();

        std::string GenerateDuplicateName(Scene* scene, const std::string& baseName);

    private:
        std::string name;
        UUID sceneID;
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<UUID> rootEntities;


        MeshRendererStorage meshRenderers;
        MeshFilterStorage meshFilters;
        LightStorage lights;
        TransformStorage transforms;
        HierarchyStorage hierarchys;
        NameTagComponentStorage nameTags;
        SkeletonComponentStorage skeletons;
        AnimationComponentStorage animations;

    };
}

