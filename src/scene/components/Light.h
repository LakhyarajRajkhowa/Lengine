#pragma once

#include <glm/glm.hpp>

namespace Lengine {
    enum class LightType {
        Directional = 0,
        Point = 1,
        Spotlight = 2,
        count = 3
    };

    class Light {
    public:
        UUID id;
        glm::vec3 color = glm::vec3(.10f, .10f, .10f);
    };



}