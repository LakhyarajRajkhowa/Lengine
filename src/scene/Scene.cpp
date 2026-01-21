#include "Scene.h"
#include <cassert>


namespace Lengine {

  

    Entity* Scene::createEntity (
        const std::string& name,
        const UUID meshID,
        EntityType type,
        UUID entityID
    ) {
        auto entity = std::make_unique<Entity>(entityID, name, type);
        Entity* entityPtr = entity.get();

        uint32_t index = entities.size();
        entities.push_back(std::move(entity));
        entities.back()->setIndex(index);

        if (type == EntityType::Light) {
            entities.back()->addLight();

        }

        MeshFilter mf;
        mf.meshID = meshID;
        meshFilters.Add(entityID, mf);

        MeshRenderer mr;
        meshRenderers.Add(entityID, mr);

        return entityPtr;
    }

    Entity* Scene::addEntity(std::unique_ptr<Entity> entity, const UUID originalEntityId)
    {
        if (!entity)
            return nullptr;

        // Ensure entity has a valid UUID
        if (entity->getID().isNull())
        {
            entity->setID(UUID());
        }

        // Assign scene index
        uint32_t index = static_cast<uint32_t>(entities.size());
        entity->setIndex(index);

        // if Type == light

        if (entity->getType() == EntityType::Light) {
            entity->addLight();
        }

        // Copy mesh renderer
        const MeshRenderer& oldMr = MeshRenderers().Get(originalEntityId);
        MeshRenderer newMr = MeshRenderer(oldMr);
        meshRenderers.Add(entity->getID(), newMr);

        // Copy mesh filter
        const MeshFilter& oldMf = MeshFilters().Get(originalEntityId);
        MeshFilter newMf = MeshFilter(oldMf);
        meshFilters.Add(entity->getID(), newMf);

        entities.push_back(std::move(entity));
        return entities.back().get();
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

        meshRenderers.Remove(id);
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

    PBRMaterialInstance& Scene::getPbrMaterialInstance(UUID id)
    {
        auto it = pbrMaterialInstances.find(id);
        assert(it != pbrMaterialInstances.end());
        return it->second;
    }

    const MaterialInstance& Scene::getMaterialInstance(UUID id) const
    {
        auto it = materialInstances.find(id);
        assert(it != materialInstances.end());
        return it->second;
    }

    const PBRMaterialInstance& Scene::getPbrMaterialInstance(UUID id) const
    {
        auto it = pbrMaterialInstances.find(id);
        assert(it != pbrMaterialInstances.end());
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

    UUID Scene::createPbrMaterialInstance(UUID baseMaterial)
    {
        UUID id;

        do {
            id = UUID();
        } while (pbrMaterialInstances.find(id) != pbrMaterialInstances.end());

        PBRMaterialInstance instance{};
        instance.baseMaterial = baseMaterial;

        pbrMaterialInstances.emplace(id, std::move(instance));
        return id;
    }





    void Scene::destroyMaterialInstance(UUID id)
    {
        auto it = materialInstances.find(id);
        if (it != materialInstances.end()) {
            materialInstances.erase(it);
        }
    }

    

    void Scene::assignDefaultPBRMaterials(const UUID entityID, Mesh* mesh)
    {
        if (!mesh)
            return;

        UUID defaultMat = MaterialID::DefaultPbr;


        auto& matUUIDs = meshRenderers.Get(entityID).materialIndexToUUID;
        auto& instIDs = meshRenderers.Get(entityID).materialIndexToInstID;


        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {

            UUID baseMat = defaultMat;

            auto it_matid = matUUIDs.find(matIndex);
            if (it_matid != matUUIDs.end() && it_matid->second != UUID::Null) {
                baseMat = it_matid->second;
            }

            UUID instID = createPbrMaterialInstance(baseMat);
            instIDs[matIndex] = instID;

        }
    }


}



