#include "AssetSerializer.h"

#include <string>

#include "../utils/filePath.h"
using namespace Lengine;

std::shared_ptr<Submesh>
SubMeshSerializer::Deserialize(const std::filesystem::path& path)
{
    LSubMeshFile file = ReadLSubMesh(path);

    std::string submeshName = ExtractNameFromPath(path);
    auto submesh = std::make_shared<Submesh>(
        submeshName,
        std::move(file.vertices),
        std::move(file.indices)
    );
    return submesh;
}
