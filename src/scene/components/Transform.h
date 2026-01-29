#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Lengine {
    struct TransformComponent
    {
        glm::vec3 localPosition{ 0.0f };
        glm::vec3 localRotation{ 0.0f }; // radians
        glm::vec3 localScale{ 1.0f };

        // cached
        glm::mat4 localMatrix{ 1.0f };
        glm::mat4 worldMatrix{ 1.0f };

        bool localDirty = true;
        bool worldDirty = true;

        glm::vec3 GetWorldPosition()
        {
            return glm::vec3(worldMatrix[3]);
        }

        glm::vec3 GetWorldScale()
        {
            return {
                glm::length(glm::vec3(worldMatrix[0])),
                glm::length(glm::vec3(worldMatrix[1])),
                glm::length(glm::vec3(worldMatrix[2]))
            };
        }


    };

}



