#include "Scene.h"

#include "utils/C++20.h"

using namespace Lengine;

  

    Entity* Scene::createEntity (
        const std::string& name,
        UUID entityID
    ) {
        auto entity = std::make_unique<Entity>(entityID, name);
        Entity* entityPtr = entity.get();

        entities.push_back(std::move(entity));

        rootEntities.push_back(entityID);

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

        // Copy mesh renderer
        if (Transforms().Has(originalEntityId)) {
            const TransformComponent& oldTrans = Transforms().Get(originalEntityId);
            TransformComponent newTrans = TransformComponent(oldTrans);
            transforms.Add(entity->getID(), newTrans);
        }

        // Copy mesh renderer
        if (MeshFilters().Has(originalEntityId)) {
            const MeshFilter& oldMf = MeshFilters().Get(originalEntityId);
            MeshFilter newMf = MeshFilter(oldMf);
            meshFilters.Add(entity->getID(), newMf);
        }

        // Copy mesh filter
        if (MeshRenderers().Has(originalEntityId)) {
            const MeshRenderer& oldMr = MeshRenderers().Get(originalEntityId);
            MeshRenderer newMr = MeshRenderer(oldMr);
            meshRenderers.Add(entity->getID(), newMr);
        }

        if (Lights().Has(originalEntityId)) {
            const Light& oldLight = Lights().Get(originalEntityId);
            Light newLight = Light(oldLight);
            lights.Add(entity->getID(), newLight);
        }
       

        entities.push_back(std::move(entity));
        return entities.back().get();
    }


    void Scene::RemoveEntity(const UUID id)
    {
        // 1️⃣ Remove from hierarchy first (important)
        if (hierarchys.Has(id))
        {
            auto& h = hierarchys.Get(id);

            // Detach children → make them roots
            for (UUID child : h.children)
            {
                if (hierarchys.Has(child))
                {
                    hierarchys.Get(child).parent = UUID::Null;
                    rootEntities.push_back(child);
                }
            }

            // Remove from parent children list
            if (h.parent != UUID::Null && hierarchys.Has(h.parent))
            {
                auto& parentH = hierarchys.Get(h.parent);
                parentH.children.erase(
                    std::remove(parentH.children.begin(),
                        parentH.children.end(),
                        id),
                    parentH.children.end()
                );
            }

            hierarchys.Remove(id);
        }

        // 2️⃣ Remove from rootEntities
        rootEntities.erase(
            std::remove(rootEntities.begin(), rootEntities.end(), id),
            rootEntities.end()
        );

        // 3️⃣ Remove entity object
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

        // 4️⃣ Remove components
        if (transforms.Has(id))     transforms.Remove(id);
        if (meshFilters.Has(id))    meshFilters.Remove(id);
        if (meshRenderers.Has(id))  meshRenderers.Remove(id);
        if (lights.Has(id))         lights.Remove(id);
    }

    void Scene::RemoveEntityRecursive(UUID id)
    {
        if (hierarchys.Has(id))
        {
            auto children = hierarchys.Get(id).children;
            for (UUID child : children)
                RemoveEntityRecursive(child);
        }

        RemoveEntity(id);
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


 
    void Scene::UpdateWorldTransformRecursive(
        UUID entityID,
        const glm::mat4& parentWorld,
        bool parentWorldDirty
    )
    {
        if (!transforms.Has(entityID)) return;

        auto& t = transforms.Get(entityID);

        if (t.localDirty)
        {
            TransformSystem::RecalculateLocalMatrix(t);
            t.localDirty = false;
        }

        bool worldDirty = parentWorldDirty || t.worldDirty;

        if (worldDirty)
        {
            t.worldMatrix = parentWorld * t.localMatrix;
            t.worldDirty = false;
        }

        if (hierarchys.Has(entityID)) {
            const auto& h = hierarchys.Get(entityID);
            for (UUID child : h.children)
                UpdateWorldTransformRecursive(child, t.worldMatrix, worldDirty);
        }
        
    }



    void Scene::UpdateWorldTransforms()
    {
        if (TransformSystem::Dirty) {
            const glm::mat4 identity(1.0f);

            for (UUID root : rootEntities)
            {
                UpdateWorldTransformRecursive(root, identity, true);
            }

            TransformSystem::Dirty = false;
        }


    }


    void Scene::Update() {
       
        UpdateWorldTransforms();
    }
    bool Scene::HasChildren(UUID entityID) const
    {
        if (!hierarchys.Has(entityID))
            return false;

        return !hierarchys.Get(entityID).children.empty();
    }

    const std::vector<UUID>& Scene::GetChildren(UUID entityID) const
    {
        static std::vector<UUID> empty;

        if (!hierarchys.Has(entityID))
            return empty;

        return hierarchys.Get(entityID).children;
    }

    void Scene::SetParent(UUID child, UUID parent)
    {
        if (!hierarchys.Has(child))
            hierarchys.Add(child);

        if (!hierarchys.Has(parent))
            hierarchys.Add(parent);

        auto& childH = hierarchys.Get(child);

        // Remove from old parent / roots
        if (childH.parent != UUID::Null)
        {
            auto& oldParentH = hierarchys.Get(childH.parent);
            std20::erase(oldParentH.children, child);
        }
        else
        {
            std20::erase(rootEntities, child);
        }

        // Attach
        childH.parent = parent;
        hierarchys.Get(parent).children.push_back(child);

        // ---- Correct dirtiness ----
        if (transforms.Has(child))
        {
            auto& t = transforms.Get(child);
            t.worldDirty = true;   // parent space changed
        }
    }


    void Scene::MakeOrphan(UUID child)
    {
        if (!hierarchys.Has(child))
            return;

        auto& h = hierarchys.Get(child);

        if (h.parent != UUID::Null)
        {
            auto& parentH = hierarchys.Get(h.parent);
            std20::erase(parentH.children, child);
        }

        h.parent = UUID::Null;
        rootEntities.push_back(child);

        if (transforms.Has(child))
            transforms.Get(child).localDirty = true;
    }




