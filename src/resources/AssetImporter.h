#pragma once
#include <filesystem>

#include "../core/paths.h"
#include "../utils/UUID.h"

namespace Lengine {

    namespace fs = std::filesystem;

    class AssetImporter
    {
    public:
        static UUID ImportMeshFile(const fs::path& externalPath);
    };

    class MeshImporter
    {
    public:
        static void Import(const fs::path& assetPath, UUID fileID);
    };

}
