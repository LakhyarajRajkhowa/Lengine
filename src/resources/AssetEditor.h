#pragma once
#include <filesystem>

#include "../resources/assetDatabase/AssetDatabase.h"
namespace Lengine {
    class MaterialCreator {
    public:
        static UUID Create(const std::string& name)
        {
            UUID id;

            std::filesystem::path libPath = Paths::GameLibrary_Assets_Material + name + ".pbrmat";

            if (std::filesystem::exists(libPath))
            {
                std::cerr << "Material already exists\n";
                return UUID::Null;
            }

            // --- Create default material ---
            json j;
            j["albedo"] = { 1.0f, 1.0f, 1.0f };
            j["metallic"] = 0.0f;
            j["roughness"] = 0.5f;
            j["ao"] = 1.0f;
            j["normalStrength"] = 1.0f;
            j["textures"] = json::object();

            std::ofstream file(libPath);
            file << std::setw(4) << j;
            file.close();

            // --- Register asset ---
            AssetMetadata meta;
            meta.uuid = id;
            meta.name = name;
            meta.type = AssetType::Material;
            meta.libraryPath = libPath;
            meta.sourcePath = ""; // <-- important
            meta.thumbnailPath = Paths::Icons + "material_icon.png";

            AssetDatabase::RegisterAsset(meta);

            return id;
        }


    };

    class MaterialSaver {
    public:
        static void Save(
            const Material& mat,
            const std::filesystem::path& libPath
        )
        {
            json j;

            j["albedo"] = { mat.albedo.r, mat.albedo.g, mat.albedo.b };
            j["metallic"] = mat.metallic;
            j["roughness"] = mat.roughness;
            j["ao"] = mat.ao;
            j["normalStrength"] = mat.normalStrength;

            j["textures"]["albedo"] = mat.map_albedo_path;
            j["textures"]["normal"] = mat.map_normal_path;
            j["textures"]["metallic"] = mat.map_metallic_path;
            j["textures"]["roughness"] = mat.map_roughness_path;
            j["textures"]["ao"] = mat.map_ao_path;
            j["textures"]["metallicRoughness"] = mat.map_metallicRoughness_path;


            std::ofstream file(libPath);
            file << std::setw(4) << j;
        }


    };
    
}
