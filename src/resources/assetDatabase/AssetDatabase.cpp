#include "AssetDatabase.h"

using namespace Lengine;

std::unordered_map<UUID, AssetMetadata> AssetDatabase::s_Metadata;
std::unordered_map<UUID, std::shared_ptr<void>> AssetDatabase::s_LoadedAssets;
bool AssetDatabase::needsUpdate = true;
std::mutex AssetDatabase::s_AssetMutex;


const AssetMetadata* AssetDatabase::GetMetadata(const UUID& id)
{
    auto it = s_Metadata.find(id);
    if (it != s_Metadata.end())
        return &it->second;   // return pointer to metadata
    return nullptr;           // not found
}


void AssetDatabase::RegisterAsset(const AssetMetadata& metadata)
{
    // ---------------- Safety checks ----------------
    if (metadata.uuid == UUID::Null)
    {
        std::cerr << "[AssetDatabase] Attempted to register asset with NULL UUID\n";
        return;
    }

    if (!std::filesystem::exists(metadata.libraryPath))
    {
        std::cerr << "[AssetDatabase] Library file does not exist: "
            << metadata.libraryPath << std::endl;
        return;
    }

    // ---------------- Normalize paths ----------------
    AssetMetadata normalized = metadata;
    normalized.libraryPath = std::filesystem::weakly_canonical(metadata.libraryPath);

    if (!metadata.sourcePath.empty())
        normalized.sourcePath = std::filesystem::weakly_canonical(metadata.sourcePath);

    // ---------------- Insert / Update ----------------
    auto it = s_Metadata.find(normalized.uuid);

    needsUpdate = true;

    if (it != s_Metadata.end())
    {
        // Reimport / overwrite
        it->second = normalized;

#ifdef _DEBUG
        std::cout << "[AssetDatabase] Updated asset: "
            << normalized.libraryPath.filename() << std::endl;
#endif
    }
    else
    {
        s_Metadata.emplace(normalized.uuid, normalized);

#ifdef _DEBUG
        std::cout << "[AssetDatabase] Registered asset: "
            << normalized.libraryPath.filename() << std::endl;
#endif
    }
}

void AssetDatabase::SaveDatabase()
{
    std::filesystem::create_directories(Paths::GameAssetDatabase);

    json j;

    for (const auto& [uuid, meta] : s_Metadata)
    {
        j["assets"].push_back({
            { "uuid", uuid.toUint64() },
            { "name", meta.name },
            { "type", AssetTypeToString(meta.type)},
            { "source", meta.sourcePath.string() },
            { "library", meta.libraryPath.string() },
            { "thumbnail", meta.thumbnailPath.string()}
            });
    }

    std::filesystem::path outputPath = std::filesystem::path(Paths::GameAssetDatabase) / "assetdb.json";

    std::ofstream out(outputPath);
    out << j.dump(4);
}

void AssetDatabase::LoadDatabase()
{
    std::filesystem::path dbPath = std::filesystem::path(Paths::GameAssetDatabase) / "assetdb.json";
    if (!std::filesystem::exists(dbPath))
        return;

    std::ifstream in(dbPath);
    json j;
    in >> j;

    s_Metadata.clear();

    for (auto& entry : j["assets"])
    {
        AssetMetadata meta;
        meta.uuid = UUID(entry["uuid"].get<uint64_t>());
        meta.name = entry["name"].get<std::string>();
        meta.type = StringToAssetType(entry["type"]);
        meta.sourcePath = entry["source"].get<std::string>();
        meta.libraryPath = entry["library"].get<std::string>();
        meta.thumbnailPath = entry["thumbnail"].get<std::string>();

        s_Metadata[meta.uuid] = meta;
    }
}


