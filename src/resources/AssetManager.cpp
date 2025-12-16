#include "AssetManager.h"

using namespace Lengine;

void AssetManager::LoadAllMetaFiles(const fs::path& root)
{
    
    
    if (!fs::exists(root))
        return;
    for (auto& entry : fs::recursive_directory_iterator(root))
    {
        if (!entry.is_regular_file())
            continue;

        const fs::path& metaPath = entry.path();

        // Must end with ".meta"
        if (metaPath.extension() != ".meta")
            continue;
        fs::path assetPath = metaPath;
        assetPath.replace_extension("");
        // Load meta (contains uuid, type, source)
        MetaFile meta = MetaFileSystem::Load(assetPath.string());


        if (assetPath.extension() == ".obj") {
            loadMesh(meta.uuid, assetPath.string());
        }
        if (assetPath.extension() == ".mtl") {
            loadMaterial(meta.uuid, assetPath.string());
        }
        
    }
}


Mesh* AssetManager::getMesh(const UUID& id) {
    auto it = meshes.find(id);
    if (it == meshes.end())
        return nullptr; 

    return it->second.get();
}


Material* AssetManager::getMaterial(const UUID& id) {
    return materials[id].get();
}
UUID AssetManager::getMeshUUID(const std::string& name) {
    for (auto& [uuid, mesh] : meshes)
    {
        if (mesh && mesh->name == name)
            return uuid;
    }

    return UUID::Null;
}
UUID AssetManager::getMaterialUUID(const std::string& name) {
    for (auto& [uuid, material] : materials)
    {
        if (material && material->name == name)
            return uuid;
    }

    return UUID::Null;
}
void AssetManager::loadMesh(const UUID& uuid, const std::string& path)
{
    std::string meshName = ExtractNameFromPath(path);
    UUID id = uuid;
    std::string newPath = StripQuotes(path);

    std::shared_ptr<Mesh> ptr;
    Model model;
    model.loadModel(meshName, newPath, ptr);
   
    meshes[uuid] = ptr;
}

// put shaders to the material
void AssetManager::loadMaterial(
    const UUID& uuid,
    const std::string& path,
    const std::string& vertexShaderPath ,
    const std::string& fragmentShaderPath 
)
{
    std::string materialName = ExtractNameFromPath(path);
    std::string shaderName = ExtractFileNameFromPath(vertexShaderPath);
    UUID id = uuid;
    std::shared_ptr<Material> materialPtr;
    GLSLProgram* shader = loadShader(shaderName, vertexShaderPath, fragmentShaderPath);
    materialPtr = std::make_shared<Material>(materialName, shader);
    materials[uuid] = materialPtr;

   
}

UUID AssetManager::importMesh(const std::string& path) {
    MetaFile meta;
    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "mesh";
        meta.source = NormalizePath(Paths::Mesh + fileName);

        MetaFileSystem::Save(path, meta);
    }
    else {
        meta = MetaFileSystem::Load(path);
    }

    return meta.uuid;
}
UUID AssetManager::importMaterial(const std::string& path) {
    MetaFile meta;
  
    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "material";
        meta.source = NormalizePath(Paths::Materials + fileName);

        MetaFileSystem::Save(path, meta);
    }
    else {
        meta = MetaFileSystem::Load(path);
    }

    return meta.uuid;
}

UUID AssetManager::importAndLoadMesh(const std::string& name, const std::string& path) {
    UUID id = importMesh(path);
    loadMesh(id, path);
    return id;
}
UUID AssetManager::importAndLoadMaterial(
    const std::string& name,
    const std::string& path,
    const std::string& vertexShaderPath ,
    const std::string& fragmentShaderPath 
) {
    
    UUID id = importMaterial(path);
    loadMaterial(id, path, vertexShaderPath, fragmentShaderPath);
    return id;
}

GLSLProgram* AssetManager::loadShader(const std::string& name,
    const std::string& vert,
    const std::string& frag)
{
    if (shaders.count(name)) return shaders[name].get();

    auto shader = std::make_unique<GLSLProgram>();
    shader->compileShaders(vert.c_str(), frag.c_str());
    shader->linkShaders();

    shaders[name] = std::move(shader);

    return shaders[name].get();
}

GLSLProgram* AssetManager::getShader(const std::string& name) {
    return shaders[name].get();
}

GLTexture* AssetManager::loadTexture(const std::string& name , const std::string& path) {
    MetaFile meta;

    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "texture";
        meta.source = NormalizePath(Paths::Shaders + fileName);
        MetaFileSystem::Save(name, meta);
    }
    else {
        meta = MetaFileSystem::Load(path);
    }

    UUID id = meta.uuid;
    std::shared_ptr<GLTexture> tex = std::make_shared<GLTexture>();
    *tex = textureCache.getTexture(path);

    textures[id] = tex;
    return tex.get();
 
}


void AssetManager::saveModelFile(const UUID& meshUUID)
{
    auto it = meshes.find(meshUUID);
    if (it == meshes.end() || !it->second) {
        std::cerr << "saveModelFile: No mesh found for UUID "
            << meshUUID.value() << std::endl;
        return;
    }

    std::shared_ptr<Mesh> meshPtr = it->second;
    Mesh* mesh = meshPtr.get();

   
    ModelFile modelFile;
    modelFile.uuid = meshUUID;                 
    modelFile.type = "model";
    modelFile.sourceMeshId = meshUUID;     

   
    for (auto& sm : mesh->subMeshes)
    {
        ModelSubmeshInfo info;
        info.name = sm.getName();
        modelFile.submeshes.push_back(info);
    }

  
    std::string fileName = mesh->name;
    modelFolderPath = Paths::ActiveGameFolder + "/internal/models";
    ModelFileSystem::Save(modelFolderPath + "/" + fileName, modelFile);

    std::cout << "Saved model file: " << fileName << ".model" << std::endl;


}
Scene* AssetManager::createScene(const std::string& name, const std::string& folderPath)
{
    namespace fs = std::filesystem;

    // Create a new scene with UUID
    UUID sID;
    Scene* scene = new Scene(name, sID);

    fs::path dir(folderPath);

    // Ensure folder exists
    if (!fs::exists(dir))
        fs::create_directories(dir);

    // File name generation
    std::string fileName = name + ".json";
    fs::path finalPath = dir / fileName;

    // --- JSON BUILDING ---
    json jScene;

    // Order: UUID → Name → Entities
    jScene["uuid"] = scene->getUUID().toUint64();
    jScene["name"] = name;
    jScene["entities"] = json::array();   // empty at creation

    // --- WRITE FILE ---
    std::ofstream file(finalPath);
    file << jScene.dump(4);

    std::cout << "Scene \"" << name << "\" created successfully at: "
        << finalPath.string() << std::endl;

    return scene;
}

void AssetManager::saveScene(const Scene& scene, const std::string& folderPath)
{
    // Build output file path automatically
    std::filesystem::path dir(folderPath);

    // Ensure folder exists
    if (!std::filesystem::exists(dir))
        std::filesystem::create_directories(dir);

    std::string sceneName = scene.getName();
    std::string fileName = sceneName + ".json";

    std::filesystem::path finalPath = dir / fileName;

    // --- JSON BUILDING ---
    json jScene;
    // Add scene UUID
    jScene["uuid"] = scene.getUUID().toUint64();
    jScene["name"] = sceneName;
    jScene["entities"] = json::array();

    const auto& entities = scene.getEntities();

    for (const auto& entityPtr : entities)
    {
        auto entity = entityPtr.get();
        json jEntity;

        jEntity["entityID"] = entity->getID().toUint64();
        jEntity["name"] = entity->getName();
        jEntity["type"] = entity->getType();
        jEntity["meshID"] = entity->getMeshID().toUint64();
        const auto& transform = entity->getTransform();

        jEntity["transform"] = {
            { "position", { transform.position.x, transform.position.y, transform.position.z }},
            { "rotation", { transform.rotation.x, transform.rotation.y, transform.rotation.z }},
            { "scale",    { transform.scale.x,    transform.scale.y,    transform.scale.z }}
        };
       
        

        jScene["entities"].push_back(jEntity);
    }

    // --- WRITE FILE ---
    std::ofstream file(finalPath.string());
    file << jScene.dump(4);

    // --- SUCCESS MESSAGE ---
    std::cout << "Saved \"" << sceneName << "\" successfully at: "
        << finalPath.string() << std::endl;
}

Scene* AssetManager::loadScene(const std::string& filePath)
{
    namespace fs = std::filesystem;

    if (!fs::exists(filePath)) {
        std::cerr << "Scene file not found: " << filePath << std::endl;
        return nullptr;
    }

    // --- READ FILE ---
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << filePath << std::endl;
        return nullptr;
    }

    json jScene;
    try {
        file >> jScene;   // this can throw
    }
    catch (const json::parse_error& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return nullptr;
    }

    // --- EXTRACT SCENE DATA ---
    std::string sceneName;
    uint64_t sceneUUID = 0;

    try {
        sceneName = jScene.at("name").get<std::string>();
        sceneUUID = jScene.at("uuid").get<uint64_t>();
    }
    catch (const json::exception& e) {
        std::cerr << "Invalid scene format: " << e.what() << std::endl;
        return nullptr;
    }

    Scene* scene = new Scene(sceneName, UUID(sceneUUID));

    // --- LOAD ENTITIES ---
    try {
        auto jEntities = jScene.at("entities");

        for (auto& jEntity : jEntities)
        {
            try {
                uint64_t entityID = jEntity.at("entityID").get<uint64_t>();
                std::string entityName = jEntity.at("name").get<std::string>();
                EntityType entityType = jEntity.at("type");
                UUID meshUUID;
                if (jEntity.contains("meshID")) {
                    meshUUID = UUID(jEntity.at("meshID").get<uint64_t>());
                }

                Entity* entity = scene->createEntity(entityName, meshUUID, UUID(entityID), entityType);

                auto t = jEntity.at("transform");
                auto pos = t.at("position");
                auto rot = t.at("rotation");
                auto scl = t.at("scale");

                Transform transform;
                transform.position = { pos[0], pos[1], pos[2] };
                transform.rotation = { rot[0], rot[1], rot[2] };
                transform.scale = { scl[0], scl[1], scl[2] };

                entity->setTransform(transform);
                
                if (meshes[meshUUID]) {
                    Mesh* mesh = getMesh(entity->getMeshID());
                    scene->assignDefaultMaterials(entity, mesh);

                }

            }
            catch (const json::exception& e) {
                std::cerr << "Skipping invalid entity in scene \"" << sceneName
                    << "\": " << e.what() << std::endl;
                continue; // skip only this entity
            }
            catch (const std::exception& e) {
                std::cerr << "Error creating entity: " << e.what() << std::endl;
                continue;
            }
        }

    }
    catch (const json::exception& e) {
        std::cerr << "Invalid entity data in scene: " << e.what() << std::endl;
        delete scene;
        return nullptr;
    }

    std::cout << "Loaded scene \"" << sceneName
        << "\" from: " << filePath << std::endl;

    return scene;
}
