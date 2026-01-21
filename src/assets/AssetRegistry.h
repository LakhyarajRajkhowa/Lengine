#pragma once

#include <filesystem>
#include <unordered_map>
#include "../utils/UUID.h"

namespace Lengine {

    enum class AssetType {
        Unknown = -1,
        Mesh = 0,
        Material_pbr = 1,
        Material = 2,
        Texture_srgb = 3,
        Texture = 4,
        Shader = 5,
        count = 6
    };

    struct AssetRecord {
        UUID uuid;
        std::filesystem::path path;
        AssetType type;
    };

    inline std::string AssetTypeToString(AssetType type) {
        switch (type) {
        case AssetType::Mesh:            return "Mesh";
        case AssetType::Material_pbr:    return "Material_pbr";
        case AssetType::Material:        return "Material";
        case AssetType::Texture:         return "Texture";
        case AssetType::Texture_srgb:    return "Texture_srgb";
        case AssetType::Shader:          return "Shader";
        default: return "Unknown";
        }
    }

    extern std::unordered_map<UUID, AssetRecord> assetRegistry;
}
