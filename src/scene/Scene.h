#pragma once

#include "../resources/TextureCache.h"

#include "../scene/Entity.h"

namespace Lengine {
    class Scene {
    public:
        
        Scene(const std::string& n, UUID sID)
            : name(n), sceneID(sID) {
        }
  
        Entity* createEntity(
            const std::string& name,
            UUID meshID
        );

        const  Entity* getEntityByName(const std::string& name) const;
        Entity* getEntityByName(const std::string& name) ;

        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }
        std::vector<std::unique_ptr<Entity>>& Scene::getEntities() {
            return entities;
        }
        void removeEntity(const std::string& name);
       

        bool showBoundingSphere = false;
        const std::string& getName() const { return name; }
        std::string getName() { return name; }
        UUID getUUID() const { return sceneID; }

    private:
        std::string name;
        UUID sceneID;
        std::vector<std::unique_ptr<Entity>> entities;

    };
}

