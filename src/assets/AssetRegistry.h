#pragma once

#include <filesystem>
#include <unordered_map>
#include "../utils/UUID.h"

namespace Lengine {

    enum class AssetType {
        Mesh = 0,
        Material = 1,
        Texture = 2,
        Shader = 3,
        count = 4
    };

    struct AssetRecord {
        UUID uuid;
        std::filesystem::path path;
        AssetType type;
    };

    inline std::string AssetTypeToString(AssetType type) {
        switch (type) {
        case AssetType::Mesh:     return "Mesh";
        case AssetType::Material: return "Material";
        case AssetType::Texture:  return "Texture";
        case AssetType::Shader:   return "Shader";
        default: return "Unknown";
        }
    }

    extern std::unordered_map<UUID, AssetRecord> assetRegistry;
}
