#include "AssetImporter.h"

#include <string>
#include <fstream>

namespace fs = std::filesystem;

using namespace Lengine;

UUID AssetImporter::ImportMeshFile(const fs::path& externalPath)
{
    if (!fs::exists(externalPath))
        throw std::runtime_error("Asset file does not exist");

    //  Decide destination inside project
    fs::path assetsRoot = Paths::GameAssets_Mesh;
    fs::path destPath = assetsRoot / externalPath.filename();

    //  Handle name collisions (tree.gltf already exists)
    int counter = 1;
    while (fs::exists(destPath))
    {
        destPath = assetsRoot /
            (externalPath.stem().string() + "_" +
                std::to_string(counter++) +
                externalPath.extension().string());
    }

    // 3 Copy file
    fs::copy_file(
        externalPath,
        destPath,
        fs::copy_options::overwrite_existing
    );

    //  Generate stable file UUID
    UUID fileID = UUID();

    //  Write .meta file
    fs::path metaPath = destPath;
    metaPath += ".meta";

    std::ofstream meta(metaPath);
    meta << "{\n";
    meta << "  \"fileID\": \"" << fileID.value() << "\",\n";
    meta << "  \"sourcePath\": \"" << destPath.generic_string() << "\",\n";
    meta << "  \"importType\": \"Mesh\"\n";
    meta << "}\n";

    meta.close();

    //  Trigger importer
   // MeshImporter::Import(destPath, fileID);

    return fileID;
}
