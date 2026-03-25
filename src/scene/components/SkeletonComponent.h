#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../utils/UUID.h"
#include "../graphics/animations/Skeleton.h"

namespace Lengine {
    struct SkeletonComponent
    {
        UUID skeletonID = UUID::Null;

       // Skeleton* skeleton = nullptr;

        std::vector<glm::mat4> localPose;
        std::vector<glm::mat4> globalPose;
        std::vector<glm::mat4> finalMatrices;

        bool dirty = true;

        SkeletonComponent() = default;

        SkeletonComponent(UUID id)
            : skeletonID(id)
        {
        }
    };
    
}
