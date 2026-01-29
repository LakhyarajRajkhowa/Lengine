#pragma once
#include <filesystem>

#include "../core/paths.h"
#include "../utils/UUID.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "../resources/assetDatabase/MeshAsset.h"
#include "../resources/assetDatabase/AssetDatabase.h"

#include "../graphics/material/MaterialLoader.h"

#include "../scene/Prefab.h"



namespace Lengine {

    namespace fs = std::filesystem;

    class AssetImporter
    {
    public:
        // mesh and texture takes import time so they are multithreaded
        // and hence UUID created early
        static bool ImportMeshFile(const fs::path& externalPath, const UUID& uuid);
        static bool ImportTextureFile(const fs::path& externalPath, const UUID& uuid);
        static bool ImportPrefabFile(const fs::path& externalPath, const UUID& uuid);


        // material importing is just the .pbrmat file 
        // takes almsot no time (ideally should use multithread here too)
        // so no need to create UUID early although we can
        static bool ImportMaterialFile(const fs::path& externalPath, const UUID& uuid);

    };

    // idk why m i even making these classes 
    // TODO : Learn Templating and implement it
    class MeshImporter
    {
    public:
        static void Import(const fs::path& assetPath, UUID fileID);
    };
    class TextureImporter
    {
    public:
        static void Import(const fs::path& assetPath, UUID fileID);
    };

    class MaterialImporter
    {
    public:
        static void Import(const fs::path& assetPath, UUID fileID);
        static void ImportAssimpMaterial(
            const aiScene* scene,
            const aiMaterial* mat,
            const fs::path& modelPath,
            const fs::path& outDir,
            UUID materialUUID
        );
    };
    class PrefabImporter
    {
    public:
        static Prefab* Import(const fs::path& assetPath, UUID fileID);
        static PrefabNode* LoadPrefabNode(
            const aiScene* scene,
            const aiNode* node,
            std::unordered_map<const aiMesh*, UUID>& loadedSubmeshes,
            std::unordered_map<const aiMaterial*, UUID>& loadedMaterials,
            const std::filesystem::path& sourceAssetPath,
            const std::filesystem::path& outDir,
            LMeshFile& lmesh
        );

    };

}
