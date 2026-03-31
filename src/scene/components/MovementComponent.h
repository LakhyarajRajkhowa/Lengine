#pragma once

#include <glm/glm.hpp>

namespace Lengine {

    struct MovementComponent
    {
        // movement parameters
        float moveSpeed = 5.0f;
        float acceleration = 20.0f;
        float friction = 8.0f;

        // physics state
        glm::vec3 velocity{ 0.0f };
        glm::vec3 desiredDirection{ 0.0f };

        bool grounded = false;

        // optional
        float jumpForce = 5.0f;
    };

}