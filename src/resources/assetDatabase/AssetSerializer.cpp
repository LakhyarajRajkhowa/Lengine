#include "AssetSerializer.h"

#include <string>

#include "../utils/filePath.h"
using namespace Lengine;

std::shared_ptr<Mesh>
SubMeshSerializer::Deserialize(const std::filesystem::path& path)
{
    LSubMeshFile file = ReadLSubMesh(path);

    std::string submeshName = ExtractNameFromPath(path);
    auto submesh = std::make_shared<Mesh>(
        submeshName,
        std::move(file.vertices),
        std::move(file.indices)
    );

    submesh->bonePalette = std::move(file.bonePalette);

    return submesh;
}
