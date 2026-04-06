#pragma once

#include <PxPhysicsAPI.h>

namespace Lengine {
    struct RigidbodyComponent
    {
        float mass = 1.0f;
        bool useGravity = false;
        bool isKinematic = false;


        bool dirty = true;

    };
}

