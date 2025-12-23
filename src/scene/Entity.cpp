#include "Entity.h" 
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/component_wise.hpp>
namespace Lengine {
    void Entity::setType(const EntityType& t) {
        if (t == EntityType::Light) {
            if (!light.has_value()) {
                addLight();
            }
        }
        else if (t == EntityType::Light) {
            if (light.has_value()) {
                
            }
        }
        type = t; 
    }

    void Entity::requestMesh(const UUID& id)
    {
        pendingMeshID = id;
        pendingMesh = true;
    }

    bool Entity::hasPendingMesh() const
    {
        return pendingMesh;
    }

    UUID Entity::getRequestedMeshID() const
    {
        return pendingMeshID;
    }

    void Entity::clearPendingMesh()
    {
        pendingMesh = false;
    }

}
