#pragma once

#include <filesystem>
#include <memory>

#include "../resources/assetDatabase/MeshAsset.h"


namespace Lengine {
    class SubMeshSerializer
    {
    public:
        static std::shared_ptr<Mesh>
            Deserialize(const std::filesystem::path& path);

        static void
            Serialize(const std::filesystem::path& path,
                const Mesh& submesh);
    };
}

