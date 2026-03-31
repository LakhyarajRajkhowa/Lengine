#include "Scene.h"

#include "utils/C++20.h"

using namespace Lengine;

  

    Entity* Scene::createEntity (
        const std::string& name,
        UUID entityID
    ) {
        auto entity = std::make_unique<Entity>(entityID);
        nameTags.Add(entityID, NameTagComponent(name));

        Entity* entityPtr = entity.get();

        entities.push_back(std::move(entity));

        rootEntities.push_back(entityID);

        return entityPtr;
    }
    UUID Scene::DuplicateEntityRecursive(UUID originalID, UUID newParent, UUID newRoot)
    {
        auto entity = std::make_unique<Entity>(UUID());

        Entity* newEntity = addEntity(std::move(entity), originalID);
        UUID newID = newEntity->getID();

        if (newRoot == UUID::Null)
            newRoot = newID;

        // hierarchy
        if (newParent != UUID::Null)
        {
            auto& h = hierarchys.Add(newID);
            h.parent = newParent;
            hierarchys.Get(newParent).children.push_back(newID);

            rootEntities.erase(
                std::remove(rootEntities.begin(), rootEntities.end(), newID),
                rootEntities.end()
            );
        }
        else
        {
            hierarchys.Add(newID);
        }

        // fix mesh filter root
        if (meshFilters.Has(newID))
        {
            meshFilters.Get(newID).rootParent = newRoot;
        }

        // duplicate children
        if (hierarchys.Has(originalID))
        {
            const auto& children = hierarchys.Get(originalID).children;

            for (UUID child : children)
            {
                DuplicateEntityRecursive(child, newID, newRoot);
            }
        }

        return newID;
    }

    UUID Scene::DuplicateHierarchy(UUID rootID)
    {
        return DuplicateEntityRecursive(rootID, UUID::Null, UUID::Null);
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

        UUID entityId = entity->getID();

        // Copy Transforms
        if (Transforms().Has(originalEntityId)) {
            const TransformComponent& oldTrans = Transforms().Get(originalEntityId);
            TransformComponent newTrans = TransformComponent(oldTrans);
            transforms.Add(entityId, newTrans);

            // offset the position a bit
            auto& t = Transforms().Get(entityId);
            glm::vec3 offset = glm::vec3{ (t.GetWorldScale().x * 1.0f), 0.0f, (t.GetWorldScale().z * 1.0f), };

            t.localPosition += offset;
            t.localDirty = true;

            TransformSystem::Dirty = true;
        }

        // Copy mesh filters
        if (MeshFilters().Has(originalEntityId)) {
            const MeshFilter& oldMf = MeshFilters().Get(originalEntityId);
            meshFilters.Add(entityId, MeshFilter(oldMf.submeshID, entityId));

            std::cout << "root: " << GetRootParent(entityId) << std::endl;

        }

        // Copy mesh Renderer
        if (MeshRenderers().Has(originalEntityId)) {
            const MeshRenderer& oldMr = MeshRenderers().Get(originalEntityId);
            MeshRenderer newMr = MeshRenderer(oldMr);
            meshRenderers.Add(entityId, newMr);
        }

        // Copy light component
        if (Lights().Has(originalEntityId)) {
            const Light& oldLight = Lights().Get(originalEntityId);
            Light newLight = Light(oldLight);
            lights.Add(entityId, newLight);
        }

        // Name tag 
        if (NameTags().Has(originalEntityId))
        {
            auto& tag = NameTags().Get(originalEntityId);
            std::string newName = GenerateDuplicateName(this, tag.name);
            NameTags().Add(entityId, NameTagComponent(newName));
        }
        
        //// -------- SKELETON (root) --------
        if (Skeletons().Has(originalEntityId)) {
            auto& sk = Skeletons().Get(originalEntityId);

           Skeletons().Add(entityId, SkeletonComponent(sk.skeletonID));

           std::cout << "actual root: " << entityId << std::endl;

        }


        //// -------- ANIMATION (root) --------

        if (Animations().Has(originalEntityId)) {
            AnimationComponent* anim = Animations().Get(originalEntityId);

           Animations().Add(entityId , AnimationComponent(anim->animationIDs));


        }

        
        entities.push_back(std::move(entity));
        rootEntities.push_back(entityId);
        return entities.back().get();
    }

    UUID Scene::GetRootParent(const UUID& entityID)
    {
        UUID currentID = entityID;

        while (Hierarchys().Has(currentID))
        {
            auto& h = Hierarchys().Get(currentID);

            if (h.parent == UUID::Null)
                break;

            currentID = h.parent;
        }

        return currentID;
    }

    void Scene::RemoveEntity(const UUID id)
    {
        // Remove from hierarchy first (important)
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

        // Remove from rootEntities
        rootEntities.erase(
            std::remove(rootEntities.begin(), rootEntities.end(), id),
            rootEntities.end()
        );

        // Remove entity object
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

        // 4Remove components
        if (transforms.Has(id))     transforms.Remove(id);
        if (meshFilters.Has(id))    meshFilters.Remove(id);
        if (meshRenderers.Has(id))  meshRenderers.Remove(id);
        if (lights.Has(id))         lights.Remove(id);
        if (nameTags.Has(id))       nameTags.Remove(id);
        if (animations.Has(id))     animations.Remove(id);
        if (cameras.Has(id))     cameras.Remove(id);


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

    std::string Scene::GenerateDuplicateName(Scene* scene, const std::string& baseName)
    {
        int counter = 1;
        std::string newName;

        while (true)
        {
            newName = baseName + "_" + std::to_string(counter);

            bool exists = false;

            for (auto& e : scene->getEntities())
            {
                if (!scene->NameTags().Has(e->getID()))
                    continue;

                auto& tag = scene->NameTags().Get(e->getID());

                if (tag.name == newName)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
                return newName;

            counter++;
        }
    }




