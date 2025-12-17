#pragma once

#include "../resources/TextureCache.h"

#include "../scene/Entity.h"
#include "../scene/EntityComponentSystem.h"
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
            UUID meshID,
            UUID entityID = UUID(),
            EntityType type = EntityType::DefaultObject
        );

        const  Entity* getEntityByName(const std::string& name) const;
        Entity* getEntityByName(const std::string& name) ;

        const  Entity* getEntityByID(const UUID& id) const;
        Entity* getEntityByID(const UUID& UUID);

        const Entity* Scene::getEntityByIndex(const uint32_t& index) const {
            return entities[index].get();
        }
        Entity* Scene::getEntityByIndex(const uint32_t& index)  {
            return entities[index].get();
        }

        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
            return entities;
        }
        void removeEntity(const UUID);
       
        /*
        // ---- MeshRenderer API ----
        void addMeshRenderer(uint32_t e, const MeshRendererComponent& mr) {
            meshRenderers[e] = mr;
        }

        bool hasMeshRenderer(uint32_t e) const {
            return meshRenderers.find(e) != meshRenderers.end();
        }

        MeshRendererComponent& getMeshRenderer(uint32_t e) {
            return meshRenderers.at(e);
        }

        const std::unordered_map<uint32_t, MeshRendererComponent>&
            getMeshRenderers() const {
            return meshRenderers;
        }
        */
        bool showBoundingSphere = false;
        const std::string& getName() const { return name; }
        std::string getName() { return name; }
        UUID getUUID() const { return sceneID; }


        MaterialInstance& getMaterialInstance(UUID id);
        const MaterialInstance& getMaterialInstance(UUID id) const;
        UUID createMaterialInstance(UUID baseMaterial);
        const UUID createMaterialInstance(UUID baseMaterial) const ;
        void destroyMaterialInstance(UUID id);
        void assignDefaultMaterials(Entity* entity,Mesh* mesh);
        const void assignDefaultMaterials(Entity* entity, Mesh* mesh) const ;
        ResolvedMaterial getMaterialForSubmesh(
            Entity* entity,
            const std::string& submeshName,
            Material* base);

    private:
        std::string name;
        UUID sceneID;
        std::vector<std::unique_ptr<Entity>> entities;
        

        std::unordered_map<UUID, MaterialInstance> materialInstances;

       // std::unordered_map<uint32_t, MeshRendererComponent> meshRenderers; // entityIdx, mrc
       

    };
}

