#include "Scene.h"
#include <cassert>


namespace Lengine {

  

    Entity* Scene::createEntity (const std::string& name, UUID meshID, UUID entityID, EntityType type) {
        auto entity = std::make_unique<Entity>(entityID, name, type, meshID);
        Entity* entityPtr = entity.get();

        uint32_t index = entities.size();
        entities.push_back(std::move(entity));
        entities.back()->setIndex(index);

        if (type == EntityType::Light) {
            entities.back()->addLight();   
        }

        return entityPtr;
    }

    void Scene::removeEntity(const UUID id)
    {
        entities.erase(
            std::remove_if(
                entities.begin(),
                entities.end(),
                [&](const std::unique_ptr<Entity>& e)
                {
                    return e->getID() == id;     
                }
            ),
            entities.end()
        );
    }


    const Entity* Scene::getEntityByName(const std::string& name) const {
        for (auto& entity : entities) {
            if (entity->getName() == name) {
                return entity.get();
            }
        }
        return nullptr;
    }
    Entity* Scene::getEntityByName(const std::string& name)  {
        for (auto& entity : entities) {
            if (entity->getName() == name) {
                return entity.get();
            }
        }
        return nullptr;
    }

    const Entity* Scene::getEntityByID(const UUID& id) const {
        for (auto& entity : entities) {
            if (entity->getID() == id) {
                return entity.get();
            }
        }
        return nullptr;
    }
    Entity* Scene::getEntityByID(const UUID& id) {
        for (auto& entity : entities) {
            if (entity->getID() == id) {
                return entity.get();
            }
        }
        return nullptr;
    }

    MaterialInstance& Scene::getMaterialInstance(UUID id)
    {
        auto it = materialInstances.find(id);
        assert(it != materialInstances.end());
        return it->second;
    }

    const MaterialInstance& Scene::getMaterialInstance(UUID id) const
    {
        auto it = materialInstances.find(id);
        assert(it != materialInstances.end());
        return it->second;
    }


    UUID Scene::createMaterialInstance(UUID baseMaterial)
    {
        UUID id;

        do {
            id = UUID();
        } while (materialInstances.find(id) != materialInstances.end());

        MaterialInstance instance{};
        instance.baseMaterial = baseMaterial;

        materialInstances.emplace(id, std::move(instance));
        return id;
    }





    void Scene::destroyMaterialInstance(UUID id)
    {
        auto it = materialInstances.find(id);
        if (it != materialInstances.end()) {
            materialInstances.erase(it);
        }
    }

    void Scene::assignDefaultMaterials(Entity* entity, Mesh* mesh)
    {
        if (!mesh)
            return;

        UUID defaultMat;

        switch (entity->getType()) {
        case EntityType::DefaultObject:
            defaultMat = MaterialID::Default;
            break;
        case EntityType::Light:
            defaultMat = MaterialID::LightSource;
            break;
        case EntityType::Camera:
            defaultMat = MaterialID::Default;
            break;
        }

        auto& matUUIDs = entity->getMaterialIndexUUIDs();
        auto& instIDs = entity->getMaterialIndexInstIDs();

        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {

            UUID baseMat = defaultMat;

            auto it = matUUIDs.find(matIndex);
            if (it != matUUIDs.end() && it->second != UUID::Null) {
                baseMat = it->second;
            }
            
            UUID instID = createMaterialInstance(baseMat);
            instIDs[matIndex] = instID;
        }
    }

   
}



