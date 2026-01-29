#pragma once
#include <glm/glm.hpp>
#include <limits>

namespace Lengine {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;

        AABB()
        {
            reset();
        }

        void reset()
        {
            min = glm::vec3(std::numeric_limits<float>::max());
            max = glm::vec3(-std::numeric_limits<float>::max());
        }

        void expand(const glm::vec3& p)
        {
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        glm::vec3 center() const
        {
            return (min + max) * 0.5f;
        }

        glm::vec3 extents() const
        {
            return (max - min) * 0.5f;
        }
    };

    struct Bounds
    {
        AABB aabb;
        glm::vec3 center{ 0.0f };
        float radius{ 0.0f }; // bounding sphere radius
    };

}
