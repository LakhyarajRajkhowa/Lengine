#pragma once
#include <vector>
#include "../utils/UUID.h"

namespace Lengine {
    struct MeshRendererComponent {
        UUID meshID;
        std::vector<UUID> materials;
        std::unordered_map<unsigned int, std::vector<unsigned int>> materialGroups;
    };
}


