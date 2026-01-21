#pragma once
#include "../utils/UUID.h"

namespace Lengine {
    struct MeshAsset
    {
        UUID meshAssetID;     // STABLE
        UUID sourceFileID;    // tree.gltf
        uint32_t subMeshIndex;
        std::string name;
    };

    
}
