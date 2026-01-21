#pragma once

#include <../graphics/material/Material.h>

namespace Lengine {
    class MeshRenderer {
    public:

        // Mesh Renderer
        // one per submesh
        std::unordered_map<unsigned int, UUID> materialIndexToInstID; // for materialInstance
        std::unordered_map<unsigned int, UUID> materialIndexToUUID; // for materials id from scene.json

        bool castShadows = true;
        bool receiveShadows = true;

        MeshRenderer() = default;
        MeshRenderer(const MeshRenderer& other)
        {
            materialIndexToUUID = other.materialIndexToUUID;
            castShadows = other.castShadows;
            receiveShadows = other.receiveShadows;
        }
    };

}
