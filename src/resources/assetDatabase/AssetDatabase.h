#pragma once

#include <filesystem>
#include <iostream>

#include "../utils/UUID.h"
#include "../assets/AssetRegistry.h"
#include "../assetDatabase/AssetSerializer.h"
#include "../core/paths.h"
#include "../external/json.hpp"

#include "../graphics/material/MaterialLoader.h"
#include "../resources/ImageLoader.h"

namespace Lengine {

    template<typename T>
    struct AssetTypeResolver;

    template<>
    struct AssetTypeResolver<Submesh>
    {
        static constexpr AssetType Type = AssetType::Submesh;
    };
    
    template<>
    struct AssetTypeResolver<Mesh>
    {
        static constexpr AssetType Type = AssetType::Mesh;
    };

    template<>
    struct AssetTypeResolver<Material>
    {
        static constexpr AssetType Type = AssetType::Material;
    };

    template<>
    struct AssetTypeResolver<ImageData>
    {
        static constexpr AssetType Type = AssetType::Texture;
    };
}

namespace Lengine {
    struct AssetMetadata
    {
        UUID uuid;
        std::string name;
        AssetType type;
        std::filesystem::path libraryPath;   // path to .lmesh or .lsubmesh
        std::filesystem::path sourcePath;    // original file (optional, for reimport)
        std::filesystem::path thumbnailPath; // editor preview image

    };
}
   
namespace Lengine {
    class AssetDatabase
    {
    public:
        static void RegisterAsset(const AssetMetadata& metadata);
        static bool IsAssetLoaded(UUID id);

        static void SaveDatabase();
        static void LoadDatabase();


        template<typename T>
        static std::shared_ptr<T> LoadAsset(UUID id)
        {
            std::lock_guard<std::mutex> lock(s_AssetMutex);

            // ---------------- Metadata check ----------------
            const AssetMetadata* meta = GetMetadata(id);
            if (!meta)
            {
                std::cout << "[AssetDatabase] Invalid asset UUID\n";
                return nullptr;
            }

            // ---------------- Type check ----------------
            if (meta->type != AssetTypeResolver<T>::Type)
            {
                std::cout << "[AssetDatabase] Asset type mismatch\n";
                return nullptr;
            }

            // ---------------- Already loaded? ----------------
          //  auto it = s_LoadedAssets.find(id);
          //  if (it != s_LoadedAssets.end())
          //  {
          //      return std::static_pointer_cast<T>(it->second);
          //  }

            // ---------------- Load from disk ----------------
            std::shared_ptr<T> asset;

            if constexpr (std::is_same_v<T, Submesh>)
            {
                asset = SubMeshSerializer::Deserialize(meta->libraryPath);
            }
            else if constexpr (std::is_same_v<T, Material>)
            {
                asset = LoadPBRMat(meta->libraryPath.string());
                
            }
            else if constexpr (std::is_same_v<T, ImageData>)
            {
                asset = ImageLoader::stbiLoader(NormalizePath(meta->libraryPath.string()));

            }
            else
            {
                static_assert(false, "Unsupported asset type");
            }

            // ---------------- Cache ----------------
           // s_LoadedAssets[id] = asset;
            return asset;
        }


        static const std::unordered_map<UUID, AssetMetadata>& GetAllAssets() { return s_Metadata; };
        static bool needsUpdate;
    
    private:
        static std::unordered_map<UUID, AssetMetadata> s_Metadata;
        static std::unordered_map<UUID, std::shared_ptr<void>> s_LoadedAssets;
    private:
        static const AssetMetadata* GetMetadata(const UUID& id);


        static std::mutex s_AssetMutex;


    };

}
    
