#pragma once

#include "../utils/UUID.h"

namespace Lengine {
    struct HierarchyComponent
    {
        UUID parent = UUID::Null;
        std::vector<UUID> children;
    };

}
