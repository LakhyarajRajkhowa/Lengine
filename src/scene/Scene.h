#pragma once

#include "../resources/TextureCache.h"

#include "../scene/Entity.h"
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

        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
            return entities;
        }
        void removeEntity(const UUID);
       

        bool showBoundingSphere = false;
        const std::string& getName() const { return name; }
        std::string getName() { return name; }
        UUID getUUID() const { return sceneID; }


        MaterialInstance& getMaterialInstance(UUID id);
        const MaterialInstance& getMaterialInstance(UUID id) const;
        UUID createMaterialInstance(UUID baseMaterial);
        void destroyMaterialInstance(UUID id);
        void assignDefaultMaterials(Entity* entity,Mesh* mesh);
        ResolvedMaterial getMaterialForSubmesh(
            Entity* entity,
            const std::string& submeshName,
            Material* base);
    private:
        std::string name;
        UUID sceneID;
        std::vector<std::unique_ptr<Entity>> entities;

        std::unordered_map<UUID, MaterialInstance> materialInstances;

    };
}

