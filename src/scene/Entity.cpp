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
}
