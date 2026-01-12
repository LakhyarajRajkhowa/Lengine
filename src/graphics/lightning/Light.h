#pragma once

#include <glm/glm.hpp>

namespace Lengine {
    enum class LightType {
        Directional = 0,
        Point = 1,
        Spotlight = 2,
        count = 3
    };

    struct Light {
        UUID id;
        LightType type = LightType::Point;
        void setType(const LightType& t) { type = t; }

        glm::vec3 position = glm::vec3(10.0f, 10.0f, 10.0f);
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);

        glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(.5f, 0.5f, 0.5f);

        float intensity = 1.0f;

        // Point light attenuation
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        // Spotlight cutoff angles (in degrees)
        float cutOffAngle = 30.0f;
        float outerCutOffAngle = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.5f;

        // For Shadow map (spotlight)
        glm::mat4 getLightProjection() {
            if (type == LightType::Directional)
                return glm::ortho(
                    -20.0f, 20.0f,
                    -20.0f, 20.0f,
                    nearPlane, farPlane
                );
            if (type == LightType::Spotlight)
                return  glm::perspective(
                    glm::radians(getFOV()),
                    1.0f,
                    nearPlane,
                    farPlane
                );
        }
        glm::vec3 getDirection() const {
            return glm::normalize(direction);
        }

        float getFOV() { return outerCutOffAngle * 2.0f; }


        glm::mat4 getLightView() {
            return glm::lookAt(
                position,
                position + getDirection(),
                glm::vec3(0, 1, 0)
            );
        }

        glm::mat4 getSpaceMatrix() {
            return getLightProjection() * getLightView();
        }

        bool castShadow = false;
    };
}