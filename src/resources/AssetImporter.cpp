#include "AssetImporter.h"

#include <string>
#include <fstream>

namespace fs = std::filesystem;

using namespace Lengine;


// u know all this Import code are almost same...should probably merge them
// TODO : Merge the import codes for reusability purposes ( DRY :/ )
bool AssetImporter::ImportMeshFile(const fs::path& externalPath, const UUID& uuid)
{
    if (!fs::exists(externalPath)) {
        std::cout << "Asset file does not exist: " << externalPath << std::endl;
        return false;
    }


    fs::path assetsRoot = Paths::GameAssets_Mesh;

    // Decide asset name
    fs::path assetName;
    if (externalPath.extension() == ".gltf")
        assetName = externalPath.parent_path().filename(); 
    else
        assetName = externalPath.stem();

    fs::path assetDir = assetsRoot / assetName;

    // Handle folder name collisions
    int counter = 1;
    while (fs::exists(assetDir))
    {
        // if already exist means already imported !
        std::cout << assetDir.filename() << " already exists: " << externalPath << std::endl;

        return false;
       // assetDir = assetsRoot /
        //    (assetName.string() + "_" + std::to_string(counter++));
    }

    fs::create_directories(assetDir);

    // Final mesh path
    fs::path destMeshPath = assetDir / externalPath.filename();


    // Copy main mesh file
    fs::copy_file(externalPath, destMeshPath);

    // ---------------- GLTF DEPENDENCIES ----------------
    if (externalPath.extension() == ".gltf")
    {
        fs::path srcDir = externalPath.parent_path();
        fs::path dstDir = assetDir;

        // Copy .bin files
        for (auto& p : fs::directory_iterator(srcDir))
        {
            if (p.path().extension() == ".bin")
            {
                fs::copy_file(
                    p.path(),
                    dstDir / p.path().filename(),
                    fs::copy_options::overwrite_existing
                );
            }
        }

        // Copy textures folder (if exists)
        fs::path srcTextures = srcDir / "textures";
        if (fs::exists(srcTextures))
        {
            fs::copy(
                srcTextures,
                dstDir / "textures",
                fs::copy_options::recursive |
                fs::copy_options::overwrite_existing
            );
        }
    }

    // ---------------- META FILE ----------------
    UUID fileID = uuid;

    fs::path metaPath = destMeshPath;
    metaPath += ".meta";

    std::ofstream meta(metaPath);
    meta << "{\n";
    meta << "  \"fileID\": \"" << fileID.value() << "\",\n";
    meta << "  \"sourcePath\": \"" << destMeshPath.generic_string() << "\",\n";
    meta << "  \"importType\": \"Mesh\"\n";
    meta << "}\n";
    meta.close();

    // ---------------- IMPORT ----------------
    MeshImporter::Import(destMeshPath, fileID);

    return true;
}



static void ProcessSubMesh(
    const aiMesh* aiMesh,
    uint32_t subMeshIndex,
    const std::filesystem::path& sourcePath,
    const std::filesystem::path& outDir,
    UUID parentMeshID,
    LMeshSubMeshRef& outRef,
    UUID submeshID = UUID()
    
    )
{
    LSubMeshFile sm;
    sm.subMeshID = submeshID;
    sm.parentMeshID = parentMeshID;
    sm.sourcePath = sourcePath.string();
    sm.subMeshIndex = subMeshIndex;

    // ---------------- Vertices ----------------
    sm.vertices.reserve(aiMesh->mNumVertices);
    sm.indices.reserve(aiMesh->mNumFaces * 3);

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex v{};

        // Position
        if (aiMesh->HasPositions() && aiMesh->mVertices)
        {
            v.position = glm::vec3(
                aiMesh->mVertices[i].x,
                aiMesh->mVertices[i].y,
                aiMesh->mVertices[i].z
            );
        }
        else
        {
            v.position = glm::vec3(0.0f);
        }

        // Normal
        if (aiMesh->HasNormals() && aiMesh->mNormals)
        {
            v.normal = glm::vec3(
                aiMesh->mNormals[i].x,
                aiMesh->mNormals[i].y,
                aiMesh->mNormals[i].z
            );
        }
        else
        {
            v.normal = glm::vec3(0.0f);
        }

        // Texture coordinates (UV0 only)
        if (aiMesh->HasTextureCoords(0) && aiMesh->mTextureCoords[0])
        {
            v.texCoord = glm::vec2(
                aiMesh->mTextureCoords[0][i].x,
                aiMesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            v.texCoord = glm::vec2(0.0f);
        }

        // Tangent & Bitangent
        if (aiMesh->HasTangentsAndBitangents() &&
            aiMesh->mTangents &&
            aiMesh->mBitangents)
        {
            v.tangent = glm::vec3(
                aiMesh->mTangents[i].x,
                aiMesh->mTangents[i].y,
                aiMesh->mTangents[i].z
            );

            v.bitangent = glm::vec3(
                aiMesh->mBitangents[i].x,
                aiMesh->mBitangents[i].y,
                aiMesh->mBitangents[i].z
            );
        }
        else
        {
            v.tangent = glm::vec3(0.0f);
            v.bitangent = glm::vec3(0.0f);
        }

        sm.vertices.push_back(v);
    }


    // ---------------- Indices ----------------
    for (uint32_t f = 0; f < aiMesh->mNumFaces; f++)
    {
        const aiFace& face = aiMesh->mFaces[f];
        for (uint32_t i = 0; i < face.mNumIndices; i++)
            sm.indices.push_back(face.mIndices[i]);
    }

    // ----------------- Name --------------------
    std::string name = "submesh_" + std::to_string(subMeshIndex);
    if (aiMesh->mName.C_Str())
    {
        name = aiMesh->mName.C_Str();
    }
    
    // ---------------- Write .lsubmesh ----------------
    std::filesystem::path subMeshPath =
        outDir / (name + ".lsubmesh");

    WriteLSubMesh(subMeshPath, sm);


    // ---------------- Register SubMesh asset ----------------
    AssetMetadata subMeshMeta;
    subMeshMeta.uuid = sm.subMeshID;
    subMeshMeta.name = name;
    subMeshMeta.type = AssetType::Submesh;
    subMeshMeta.libraryPath = subMeshPath;      // full path is better
    subMeshMeta.sourcePath = sourcePath;
    subMeshMeta.thumbnailPath = Paths::Icons + "submesh_icon.png";

    AssetDatabase::RegisterAsset(subMeshMeta);

    // ---------------- Fill lmesh reference ----------------
    outRef.subMeshID = sm.subMeshID;
    outRef.subMeshIndex = subMeshIndex;
    outRef.subMeshPath = subMeshPath.filename();
}

void MeshImporter::Import(const std::filesystem::path& meshPath, UUID sourceFileID)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        meshPath.string(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );


    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || !scene->HasMeshes()) {
        std::cout << "Assimp error: "<< meshPath<<" :" << importer.GetErrorString() << std::endl;
        return ;
    }


    // Output directory
    std::filesystem::path assetName =
        meshPath.parent_path().filename();  

    std::filesystem::path outDir =
        Paths::GameLibrary_Assets_Mesh / assetName;

    std::filesystem::create_directories(outDir);


    // ---------------- Create lmesh ----------------
    LMeshFile lmesh;
    lmesh.meshID = UUID();
    lmesh.sourcePath = meshPath.string();

    lmesh.subMeshes.reserve(scene->mNumMeshes);

    // ---------------- Submeshes ----------------
    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        LMeshSubMeshRef ref{};
        ProcessSubMesh(
            scene->mMeshes[i],
            i,
            meshPath,
            outDir,
            lmesh.meshID,
            ref
        );

        lmesh.subMeshes.push_back(ref);
    }

    // ----------------- Name --------------------
    std::string name = meshPath.stem().string();

    // ---------------- Write .lmesh ----------------
    std::filesystem::path lmeshPath =
        outDir / (name + ".lmesh");

    WriteLMesh(lmeshPath, lmesh);

    // ---------------- Register Mesh asset ----------------
    AssetMetadata meshMeta;
    meshMeta.uuid = lmesh.meshID;
    meshMeta.name = name;
    meshMeta.type = AssetType::Mesh;
    meshMeta.libraryPath = lmeshPath;
    meshMeta.sourcePath = meshPath;
    meshMeta.thumbnailPath = Paths::Icons + "mesh_icon.png";


    AssetDatabase::RegisterAsset(meshMeta);

}

bool AssetImporter::ImportPrefabFile(const fs::path& externalPath, const UUID& uuid)
{
    if (!fs::exists(externalPath)) {
        std::cout << "Asset file does not exist: " << externalPath << std::endl;
        return false;
    }


    fs::path assetsRoot = Paths::GameAssets_Prefab;

    // Decide asset name
    fs::path assetName;
    if (externalPath.extension() == ".gltf")
        assetName = externalPath.parent_path().filename();
    else
        assetName = externalPath.stem();

    fs::path assetDir = assetsRoot / assetName;

    // Handle folder name collisions
    int counter = 1;
    while (fs::exists(assetDir))
    {
        // if already exist means already imported !
        std::cout << assetDir.filename() << " already exists: " << externalPath << std::endl;

        return false;
        // assetDir = assetsRoot /
         //    (assetName.string() + "_" + std::to_string(counter++));
    }

    fs::create_directories(assetDir);

    // Final mesh path
    fs::path destPath = assetDir / externalPath.filename();


    // Copy main mesh file
    fs::copy_file(externalPath, destPath);

    // ---------------- GLTF DEPENDENCIES ----------------
    if (externalPath.extension() == ".gltf")
    {
        fs::path srcDir = externalPath.parent_path();
        fs::path dstDir = assetDir;

        // Copy .bin files
        for (auto& p : fs::directory_iterator(srcDir))
        {
            if (p.path().extension() == ".bin")
            {
                fs::copy_file(
                    p.path(),
                    dstDir / p.path().filename(),
                    fs::copy_options::overwrite_existing
                );
            }
        }

        // Copy textures folder (if exists)
        fs::path srcTextures = srcDir / "textures";
        if (fs::exists(srcTextures))
        {
            fs::copy(
                srcTextures,
                dstDir / "textures",
                fs::copy_options::recursive |
                fs::copy_options::overwrite_existing
            );
        }
    }

    // ---------------- META FILE ----------------
    UUID fileID = uuid;

    fs::path metaPath = destPath;
    metaPath += ".meta";

    std::ofstream meta(metaPath);
    meta << "{\n";
    meta << "  \"fileID\": \"" << fileID.value() << "\",\n";
    meta << "  \"sourcePath\": \"" << destPath.generic_string() << "\",\n";
    meta << "  \"importType\": \"Prefab\"\n";
    meta << "}\n";
    meta.close();

    // ---------------- IMPORT ----------------
    Prefab* prefab = PrefabImporter::Import(destPath, fileID);

    // Save to library/prefab/prefabname.prefab
    SavePrefab(*prefab);
    
    // Register to AssetLibrary
    AssetMetadata metaData;
    metaData.uuid = prefab->id;
    metaData.name = assetName.string();
    metaData.sourcePath = destPath;
    metaData.libraryPath = prefab->path;
    metaData.type = AssetType::Prefab;
    metaData.thumbnailPath = Paths::Icons + "prefab_icon.png";

    AssetDatabase::RegisterAsset(metaData);

    return true;
}

Prefab* PrefabImporter::Import(const std::filesystem::path& assetPath, UUID sourceFileID)
{
    
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        assetPath.string(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices
    );
   


    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || !scene->HasMeshes()) {
        std::cout << "Assimp error: " << assetPath << " :" << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    // Output directory
    std::filesystem::path assetName =
        assetPath.parent_path().filename();

    std::filesystem::path outDir =
        Paths::GameLibrary_Assets_Prefab / assetName;

    std::filesystem::path outMeshDir =
        Paths::GameLibrary_Assets_Mesh / assetName;


    std::filesystem::create_directories(outMeshDir);



    std::filesystem::path prefabPath = outDir / assetName.replace_extension(".prefab");
        

    // If file is a valid prefab file then proceed
    Prefab* prefab = new Prefab();
    prefab->id = sourceFileID;
    prefab->name = scene->mName.C_Str();
    prefab->path = prefabPath;

  

    std::filesystem::create_directories(outDir);


    // ---------------- Create lmesh ----------------
    // One Prefab = One Mesh but multiple submeshes
    LMeshFile lmesh;
    lmesh.meshID = prefab->id;
    lmesh.sourcePath = assetPath.string();
    lmesh.subMeshes.reserve(scene->mNumMeshes);

    // ---------------- Nodes ----------------
    
    // send root node of the scene for processing
    prefab->rootPrefabNode = LoadPrefabNode(
        scene,
        scene->mRootNode,
        prefab->loadedSubmeshes,
        prefab->importedMaterials,
        assetPath,
        outDir,
        lmesh
    );



    // ---------------- Write .lmesh ----------------
    std::filesystem::path lmeshPath =
        outMeshDir / (prefab->name + ".lmesh");

    WriteLMesh(lmeshPath, lmesh);

    // ---------------- Register Mesh asset ----------------
    AssetMetadata meshMeta;
    meshMeta.uuid = prefab->id;
    meshMeta.name = prefab->name;
    meshMeta.type = AssetType::Mesh;
    meshMeta.libraryPath = lmeshPath;
    meshMeta.sourcePath = assetPath;
    meshMeta.thumbnailPath = Paths::Icons + "mesh_icon.png";

    return prefab;

}

PrefabNode* PrefabImporter::LoadPrefabNode(
    const aiScene* scene,
    const aiNode* node,
    std::unordered_map<const aiMesh*, UUID>& loadedSubmeshes,
    std::unordered_map<const aiMaterial*, UUID>& loadedMaterials,
    const std::filesystem::path& sourceAssetPath,
    const std::filesystem::path& outDir,
    LMeshFile& lmesh
)


{
    std::filesystem::path assetName =
        sourceAssetPath.parent_path().filename();

    std::filesystem::path outMeshDir =
        Paths::GameLibrary_Assets_Mesh / assetName;

    std::filesystem::path outMaterialDir =
        Paths::GameLibrary_Assets_Material / assetName;

    PrefabNode* parentNode = new PrefabNode();
    parentNode->name = node->mName.C_Str();
    parentNode->parent = nullptr;

    // ---- Transform ----
    aiMatrix4x4 m = node->mTransformation;
    parentNode->localTransform = glm::transpose(glm::make_mat4(&m.a1));

    // ---- Meshes ----
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        uint32_t submeshIndex = node->mMeshes[i];
        const aiMesh* submesh = scene->mMeshes[submeshIndex];

        const aiMaterial* aiMat =
            scene->mMaterials[submesh->mMaterialIndex];

        UUID materialUUID;
        UUID submeshUUID;

        auto it = loadedSubmeshes.find(submesh);
        if (it == loadedSubmeshes.end())
        {
            submeshUUID = UUID();

            LMeshSubMeshRef ref{};
            ProcessSubMesh(
                submesh,
                submeshIndex,
                sourceAssetPath,
                outMeshDir,
                lmesh.meshID,
                ref,
                submeshUUID
            );

            lmesh.subMeshes.push_back(ref);
            loadedSubmeshes[submesh] = submeshUUID;
        }
        else
        {
            submeshUUID = it->second;
        }

        auto mit = loadedMaterials.find(aiMat);
        if (mit == loadedMaterials.end())
        {
            materialUUID = UUID();

            MaterialImporter::ImportAssimpMaterial(
                scene,
                aiMat,
                sourceAssetPath,
                outMaterialDir,
                materialUUID
            );

            loadedMaterials[aiMat] = materialUUID;
        }
        else
        {
            materialUUID = mit->second;
        }


        PrefabNode* meshNode = new PrefabNode();
        meshNode->name = parentNode->name + "_submesh_" + std::to_string(i);
        meshNode->meshID = submeshUUID;
        meshNode->parent = parentNode;
        meshNode->materialID = materialUUID;
        parentNode->children.push_back(meshNode);
    }

    // ---- Children ----
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        PrefabNode* child = LoadPrefabNode(
            scene,
            node->mChildren[i],
            loadedSubmeshes,
            loadedMaterials,
            sourceAssetPath,
            outDir,
            lmesh
        );

        child->parent = parentNode;
        parentNode->children.push_back(child);
    }

    return parentNode;
}



bool AssetImporter::ImportMaterialFile(const fs::path& externalPath, const UUID& uuid) {
    if (!fs::exists(externalPath)) {
        std::cout << "Asset file does not exist: " << externalPath << std::endl;
        return false;
    }

    if (externalPath.extension() != ".pbrmat") {
        std::cout << "Wrong file type: " << externalPath << std::endl;
        return false;
    }

    fs::path assetsRoot = Paths::GameAssets_Materials;

    // Decide asset name
    fs::path assetName;
  
    assetName = externalPath.stem();

    fs::path assetDir = assetsRoot / assetName;

    // Handle folder name collisions
    int counter = 1;
    while (fs::exists(assetDir))
    {
        // if already exist means already imported !
        std::cout << assetDir.filename() << " already exists: " << externalPath << std::endl;
        return UUID::Null;

    }

    fs::create_directories(assetDir);

    // Final  path
    fs::path destMaterialPath = assetDir / externalPath.filename();


    // Copy main  file
    fs::copy_file(externalPath, destMaterialPath);

    UUID fileID = uuid;

    fs::path metaPath = destMaterialPath;
    metaPath += ".meta";

    std::ofstream meta(metaPath);
    meta << "{\n";
    meta << "  \"fileID\": \"" << fileID.value() << "\",\n";
    meta << "  \"sourcePath\": \"" << destMaterialPath.generic_string() << "\",\n";
    meta << "  \"importType\": \"Material\"\n";
    meta << "}\n";
    meta.close();

    // ---------------- IMPORT ----------------
    MaterialImporter::Import(destMaterialPath, fileID);

    return true;

}

bool AssetImporter::ImportTextureFile(const fs::path& externalPath, const UUID& fileID) {
    if (!fs::exists(externalPath)) {
        std::cout << "Asset file does not exist: " << externalPath << std::endl;
        return false;
    }

    // check if this texture is from a prefab
    // if it is from prefab then dont copy to Paths::GameAssets_Textures
    
    fs::path destPath;

    fs::path prefabRoot =
        fs::weakly_canonical(
            Paths::GameAssets_Prefab
        );

    fs::path filePath =
        fs::weakly_canonical(
            externalPath
        );

    if (IsInsideDirectory(filePath, prefabRoot)) {
        destPath = externalPath;
    }
    else {
        fs::path assetsRoot = Paths::GameAssets_Textures;

        // Decide asset name
        fs::path assetName;

        assetName = externalPath.stem();

        fs::path assetDir = assetsRoot / assetName;

        // Handle folder name collisions
        int counter = 1;
        while (fs::exists(assetDir))
        {
            // if already exist means already imported !
            std::cout << assetDir.filename() << " already exists: " << externalPath << std::endl;
            return true;

        }

        fs::create_directories(assetDir);

        // Final  path
        fs::path destPath = assetDir / externalPath.filename();


        // Copy main  file
        fs::copy_file(externalPath, destPath);


        fs::path metaPath = destPath;
        metaPath += ".meta";

        std::ofstream meta(metaPath);
        meta << "{\n";
        meta << "  \"fileID\": \"" << fileID.value() << "\",\n";
        meta << "  \"sourcePath\": \"" << destPath << "\",\n";
        meta << "  \"importType\": \"Texture\"\n";
        meta << "}\n";
        meta.close();
    }

    
  
    // ---------------- IMPORT ----------------
   TextureImporter::Import(destPath, fileID);

    return true;

}

void MaterialImporter::Import(const fs::path& assetPath, UUID fileID) {
    std::string name = assetPath.stem().string();


    std::filesystem::path libPath = Paths::GameLibrary_Assets_Material + name + ".pbrmat";

    // copy the file to lib path
    fs::copy_file(assetPath, libPath);


    AssetMetadata materialMeta;
    materialMeta.uuid = fileID;
    materialMeta.name = name;
    materialMeta.type = AssetType::Material;
    materialMeta.libraryPath = libPath;
    materialMeta.sourcePath = assetPath;
    materialMeta.thumbnailPath = Paths::Icons + "material_icon.png";

    AssetDatabase::RegisterAsset(materialMeta);
}

void MaterialImporter::ImportAssimpMaterial(
    const aiScene* scene,
    const aiMaterial* mat,
    const fs::path& modelPath,
    const fs::path& outDir,
    UUID materialUUID
)
{
    std::string matName = mat->GetName().C_Str();
    if (matName.empty())
        matName = "Material_" + materialUUID.toUint64();

    fs::path matPath = outDir / (matName + ".pbrmat");

    // Create base material file if not present
    CreatePBRMaterial(matName, outDir);

    json j;
    {
        std::ifstream in(matPath);
        in >> j;
    }

    // ------------------------------------------------------------
    // Collect texture paths
    // ------------------------------------------------------------
    fs::path albedoPath;
    fs::path normalPath;
    fs::path metallicPath;
    fs::path roughnessPath;
    fs::path aoPath;

    auto getTexPath = [&](aiTextureType type, fs::path& outPath)
        {
            aiString texPath;
            if (mat->GetTexture(type, 0, &texPath) == AI_SUCCESS)
            {
                if (texPath.C_Str()[0] == '*')
                    return; // embedded texture (skip for now)

                outPath = fs::weakly_canonical(
                    modelPath.parent_path() / texPath.C_Str()
                );
            }
        };

    getTexPath(aiTextureType_BASE_COLOR, albedoPath);
    getTexPath(aiTextureType_NORMALS, normalPath);
    getTexPath(aiTextureType_METALNESS, metallicPath);
    getTexPath(aiTextureType_DIFFUSE_ROUGHNESS, roughnessPath);
    getTexPath(aiTextureType_AMBIENT_OCCLUSION, aoPath);

    // ------------------------------------------------------------
    // Detect ARM (metallic / roughness / ao share same texture)
    // ------------------------------------------------------------
    bool useARM = false;
    fs::path armPath;

    if (!metallicPath.empty() &&
        (metallicPath == roughnessPath || metallicPath == aoPath))
    {
        useARM = true;
        armPath = metallicPath;
    }
    else if (!roughnessPath.empty() && roughnessPath == aoPath)
    {
        useARM = true;
        armPath = roughnessPath;
    }

    // ------------------------------------------------------------
    // Write texture section
    // ------------------------------------------------------------
    if (!albedoPath.empty())
        j["textures"]["albedo"] = albedoPath.string();

    if (!normalPath.empty())
        j["textures"]["normal"] = normalPath.string();

    if (useARM)
    {
        j["textures"]["metallicRoughness"] = armPath.string();
    }
    else
    {
        if (!metallicPath.empty())
            j["textures"]["metallic"] = metallicPath.string();

        if (!roughnessPath.empty())
            j["textures"]["roughness"] = roughnessPath.string();

        if (!aoPath.empty())
            j["textures"]["ao"] = aoPath.string();
    }

    // ------------------------------------------------------------
    // Scalar fallbacks
    // ------------------------------------------------------------
    aiColor4D baseColor;
    if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &baseColor) == AI_SUCCESS)
    {
        j["albedo"] = { baseColor.r, baseColor.g, baseColor.b };
    }

    float metallic = 0.0f;
    mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
    j["metallic"] = metallic;

    float roughness = 0.5f;
    mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
    j["roughness"] = roughness;

    // ------------------------------------------------------------
    // Save material file
    // ------------------------------------------------------------
    {
        std::ofstream out(matPath);
        out << std::setw(4) << j;
    }

    // ------------------------------------------------------------
    // Register material asset
    // ------------------------------------------------------------

    fs::path metaPath = matPath;
    metaPath += ".meta";

    std::ofstream meta(metaPath);
    meta << "{\n";
    meta << "  \"fileID\": \"" << materialUUID.value() << "\",\n";
    meta << "  \"sourcePath\": \"" << matPath.generic_string() << "\",\n";
    meta << "  \"importType\": \"Material\"\n";
    meta << "}\n";
    meta.close();


    AssetMetadata materialMeta;
    materialMeta.uuid = materialUUID;
    materialMeta.name = matName;
    materialMeta.type = AssetType::Material;
    materialMeta.libraryPath = matPath;
    materialMeta.sourcePath = outDir;
    materialMeta.thumbnailPath = Paths::Icons + "material_icon.png";

    AssetDatabase::RegisterAsset(materialMeta);

}


void TextureImporter::Import(const fs::path& assetPath, UUID fileID)
{
    std::string name = assetPath.stem().string();
    auto ext = assetPath.extension();

    fs::path libPath =
        Paths::GameLibrary_Assets_Texture + (name + ext.string());

    // ---- CHECK ----
    if (fs::exists(libPath))
    {
        // Optional: log or early out
        std::cout << "Texture already exists in library: "
            << libPath << "\n";

        // You may still want to register metadata if missing
        // or simply return
        return;
    }

    // ---- COPY ----
    fs::copy_file(
        assetPath,
        libPath,
        fs::copy_options::overwrite_existing // safe even with check
    );

    AssetMetadata meta;
    meta.uuid = fileID;
    meta.name = name;
    meta.type = AssetType::Texture;
    meta.libraryPath = libPath;
    meta.sourcePath = assetPath;
    meta.thumbnailPath = Paths::Icons + "texture_icon.png";

    AssetDatabase::RegisterAsset(meta);
}

