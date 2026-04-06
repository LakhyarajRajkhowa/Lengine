#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <PxPhysicsAPI.h>

namespace Lengine {

    struct ColliderShape
    {
        enum class Type
        {
            Box,
            Sphere,
            Capsule
        };

        Type type = Type::Box;

        glm::vec3 size = { 1,1,1 };
        float radius = 0.5f;
        float height = 1.0f;

        bool isTrigger = false;

        physx::PxShape* runtimeShape = nullptr;

        bool dirty = true;
    };

    struct ColliderComponent
    {
        ColliderComponent() = default;
        ColliderComponent(std::vector<ColliderShape> shapes) : shapes(shapes) {}
        std::vector<ColliderShape> shapes;
    };

}