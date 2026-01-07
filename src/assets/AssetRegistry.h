#pragma once

#include <filesystem>
#include <unordered_map>
#include "../utils/UUID.h"

namespace Lengine {

    enum class AssetType {
        Unknown = -1,
        Mesh = 0,
        Material = 1,
        Texture_srgb = 2,
        Texture = 3,
        Shader = 4,
        count = 5
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
        case AssetType::Texture_srgb:  return "Texture_srgb";
        case AssetType::Shader:   return "Shader";
        default: return "Unknown";
        }
    }

    extern std::unordered_map<UUID, AssetRecord> assetRegistry;
}
