#pragma once

#include <../graphics/material/Material.h>

namespace Lengine {
    class MeshRenderer {
    public:

        MaterialInstance inst;

        bool castShadows = true;
        bool receiveShadows = true;

        bool render = true;

    };

}
