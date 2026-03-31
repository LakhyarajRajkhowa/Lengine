#pragma once

namespace Lengine {

    enum class ControllerType
    {
        None = 0,
        Player,
        AI
    };

    struct ControllerComponent
    {
        ControllerType type = ControllerType::None;

        // optional input data
        float moveX = 0.0f;
        float moveY = 0.0f;

        bool jump = false;
    };

}