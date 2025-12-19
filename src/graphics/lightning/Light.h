#pragma once

#include <glm/glm.hpp>

namespace Lengine {
    enum class LightType {
        Directional = 0,
        Point = 1,
        Spotlight = 2
    };

    struct Light {
        LightType type = LightType::Directional;
        glm::vec3 position = glm::vec3(10.0f, 10.0f, 10.0f);
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);

        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(.5f, 0.5f, 0.5f);
    };
}
