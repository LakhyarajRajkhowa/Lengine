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


Material* AssetManager::getMaterial(const UUID& id)
{
    auto it = materials.find(id);
    if (it == materials.end())
        return nullptr;

    return it->second.get();
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
void AssetManager::requestMeshLoad(const UUID& uuid, const std::string& path)
{
    std::string meshName = ExtractNameFromPath(path);
    auto mesh = std::make_shared<Mesh>();
    mesh->name = meshName;
    mesh->path = path;

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[uuid] = AssetState::Loading;
        meshes[uuid] = mesh;   // STORE IT
    }

    std::string cleanPath = StripQuotes(path);

    std::thread([this, uuid, mesh, cleanPath]() {

        bool ok = assimpLoader(cleanPath, *mesh);

        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[uuid] = ok ? AssetState::LoadedToCPU
            : AssetState::Failed;

        }).detach();
}

void AssetManager::processGpuUploads()
{
    std::lock_guard<std::mutex> lock(assetMutex);

    for (auto& [id, state] : assetStates) {
        if (state == AssetState::LoadedToCPU) {

            auto& mesh = meshes[id];
            for (auto& sm : mesh->subMeshes)
                sm.setupMesh(); 

            state = AssetState::LoadedToGPU;

        }
    }
}

void AssetManager::syncAssetsToScene(Scene& activeScene)
{
    // sync mesh
    for (auto& e : activeScene.getEntities())
    {
        Entity* entity = e.get();

        if (!entity->hasPendingMesh())
            continue;

        UUID id = entity->getRequestedMeshID();

        if (assetStates[id] == AssetState::LoadedToGPU)
        {
            Mesh* mesh = getMesh(id);
            entity->setMeshID(id);
            activeScene.assignDefaultMaterials(entity, mesh);
            entity->clearPendingMesh();

            assetStates[id] == AssetState::Loaded;
        }
    }
}

bool AssetManager::hasLoadingAssets() 
{
    for (const auto& [id, state] : assetStates)
    {
        if (state == AssetState::Loading)
        {
            currentLoadingAsset = id;
            return true;
        }
    }
    currentLoadingAsset = UUID::Null;
    return false;
}


float AssetManager::getLoadingProgress(const UUID& id) const
{
    auto it = assetStates.find(id);
    if (it == assetStates.end())
        return -1.0f;

    AssetState state = it->second;
    switch (state) {
    case AssetState::Failed:
        return -1.0f;
    case AssetState::Loading:
        return 0.25f;
    case AssetState::LoadedToCPU:
        return 0.50f;
    case AssetState::LoadedToGPU:
        return 0.75f;
    case AssetState::Loaded:
        return 1.0f;
    case AssetState::Unloaded:
        return -1.0f;
    default:
        return 0.0f;
    }

}

void AssetManager::drawLoadingScreens() {

    // Progress bar
    if (hasLoadingAssets()) {
        UUID id = getCurrentLoadingAsset();
        AssetType type = getAssetType(id);
        std::string assetPath = "Unknown";
        float progess = getLoadingProgress(id);

        LoadingSystem::LoadingUIConfig config;
       

        switch (type) {
        case AssetType::Mesh:
            assetPath = getMesh(id)->path;
            config.title = "Loading Mesh";
            config.message = "Loading :" + assetPath;
            LoadingSystem::progressBarUI(progess, config);
        }
    }
}

AssetType AssetManager::getAssetType(const UUID& id) {
   
    if (getMesh(id)) return AssetType::Mesh;
    else if (getMaterial(id)) return AssetType::Material;
    else if (getTexture(id)) return AssetType::Texture;
    else return AssetType::Unknown;
}

void AssetManager::loadMesh(const UUID& uuid, const std::string& path)
{
    std::string meshName = ExtractNameFromPath(path);
    UUID id = uuid;
    std::string newPath = StripQuotes(path);

    std::shared_ptr<Mesh> ptr;
    MeshRendererComponent mrc;
    Model model;

    model.loadModel(meshName, newPath, ptr);

    meshes[uuid] = ptr;
    
    addAsset(uuid, AssetType::Mesh, newPath);
}

// put shaders to the material
bool AssetManager::loadMaterial(
    const UUID& uuid,
    const std::string& path,
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath
)
{
    if (materials[uuid]) return false;
    std::string materialName = ExtractNameFromPath(path);
    std::string shaderName = ExtractFileNameFromPath(vertexShaderPath);

    GLSLProgram* shader = loadShader(shaderName, vertexShaderPath, fragmentShaderPath);

    auto materialPtr = std::make_shared<Material>(materialName, shader);
    std::string folderPath = GetFolderPath(path);
    std::string newPath = StripQuotes(path);
    bool matLoaded = loadMTL(newPath, *materialPtr);
    
    if (matLoaded) {
        std::cout << "Material Loaded: " << path << std::endl;
    }
    else {
        std::cout << "Failed to load Material: " << path << std::endl;
        return false;
    }

    if (!materialPtr->map_Kd_path.empty()) {
        std::string texName = ExtractNameFromPath(materialPtr->map_Kd_path);
        materialPtr->map_Kd = importTexture(folderPath + "/" + materialPtr->map_Kd_path);
        loadTexture(materialPtr->map_Kd, folderPath + "/" + materialPtr->map_Kd_path);
    }
    if (!materialPtr->map_Ks_path.empty()) {
        std::string texName = ExtractNameFromPath(materialPtr->map_Ks_path);
        materialPtr->map_Ks = importTexture(folderPath + "/" + materialPtr->map_Ks_path);
        loadTexture(materialPtr->map_Ks, folderPath + "/" + materialPtr->map_Ks_path);
    }
    if (!materialPtr->map_bump_path.empty()) {
        std::string texName = ExtractNameFromPath(materialPtr->map_bump_path);
        materialPtr->map_bump = importTexture(folderPath + "/" + materialPtr->map_bump_path);
        loadTexture(materialPtr->map_bump, folderPath + "/" + materialPtr->map_bump_path);
    }

    materials[uuid] = materialPtr;

    addAsset(uuid, AssetType::Material, newPath);
    return true;
}



UUID AssetManager::importMaterial(const std::string& path) {
    MetaFile meta;
  
    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "material";
        meta.source = NormalizePath(path);

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
UUID AssetManager::importMesh(const std::string& path) {
    MetaFile meta;
    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "mesh";
        meta.source = NormalizePath(path);

        MetaFileSystem::Save(path, meta);
    }
    else {
        meta = MetaFileSystem::Load(path);
    }

    return meta.uuid;
}

UUID AssetManager::importTexture(const std::string& path) {
    MetaFile meta;

    std::string fileName = ExtractFileNameFromPath(path);
    if (!MetaFileSystem::HasMeta(path)) {
        meta.uuid = UUID();
        meta.type = "texture";
        meta.source = NormalizePath(path);
        MetaFileSystem::Save(path, meta);
    }
    else {
        meta = MetaFileSystem::Load(path);
    }
    return meta.uuid;

}

void AssetManager::loadTexture(const UUID& uuid, const std::string& path) {
   
    std::string textureName = ExtractNameFromPath(path);
    UUID id = uuid;
    std::shared_ptr<GLTexture> tex = std::make_shared<GLTexture>();
    std::string newPath = StripQuotes(path);
    *tex = textureCache.getTexture(newPath);

    if (tex) {
        std::cout << "Texture Loaded: " << path << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        return;
    }

    tex->name = textureName;
    textures[id] = tex; 
     
}
GLTexture* AssetManager::getTexture(const UUID& id) {
    return textures[id].get();
}

GLTexture* AssetManager::loadImage(const std::string& name, const std::string& path) {
   
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

const AssetRecord* AssetManager::getRecord(const UUID& uuid) const
{
    auto it = assetRegistry.find(uuid);
    if (it == assetRegistry.end())
        return nullptr;

    return &it->second;
}

bool AssetManager::loadAssetFromPath(const AssetRecord& record)
{
    switch (record.type)
    {
    case AssetType::Mesh:
        loadMesh(record.uuid, record.path.string());
        return true;

    case AssetType::Texture:
        loadTexture(record.uuid, record.path.string());
        return true;

    case AssetType::Material:
        loadMaterial(record.uuid, record.path.string());
        return true;

    case AssetType::Shader:
    //    loadShader(record.uuid, record.path.string());
        return true;
    }

    return false;
}

bool AssetManager::loadAssetRegistry(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open AssetRegistry: " << filePath << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    }
    catch (const json::parse_error& e) {
        std::cerr << "AssetRegistry parse error: " << e.what() << std::endl;
        return false;
    }

    if (!j.contains("assets"))
        return false;

    for (const auto& jAsset : j["assets"])
    {
        UUID uuid(jAsset.at("uuid").get<uint64_t>());
        std::string typeStr = jAsset.at("type").get<std::string>();
        std::string path = jAsset.at("path").get<std::string>();

        AssetType type;
        if (typeStr == "Mesh")        type = AssetType::Mesh;
        else if (typeStr == "Texture") type = AssetType::Texture;
        else if (typeStr == "Material")type = AssetType::Material;
        else if (typeStr == "Shader")  type = AssetType::Shader;
        else continue; // unknown type

        assetRegistry.try_emplace(
            uuid,
            AssetRecord{ uuid, NormalizePath(path), type }
        );
    }

    return true;
}
void AssetManager::addAsset(
    const UUID& uuid,
    AssetType type,
    const std::string& path
)
{
    std::string normalizedPath = NormalizePath(path);

    // If asset already registered, do nothing
    if (assetRegistry.find(uuid) != assetRegistry.end())
        return;

    AssetRecord record;
    record.uuid = uuid;
    record.type = type;
    record.path = normalizedPath;

    assetRegistry.emplace(uuid, std::move(record));
}

void AssetManager::saveSceneAssetRegistryForScene(
    const Scene& scene,
    const std::string& folderPath
) {
    namespace fs = std::filesystem;

    fs::path dir(folderPath);
    if (!fs::exists(dir))
        fs::create_directories(dir);

    // Build per-scene registry filename
    std::string registryFileName = "assetRegistry_" + scene.getName() + ".json";
    fs::path registryPath = dir / registryFileName;

    json jRegistry;
    jRegistry["assets"] = json::array();

    std::unordered_set<UUID> usedAssets;

    // Collect asset UUIDs used by the scene
    for (const auto& entityPtr : scene.getEntities()) {
        Entity* entity = entityPtr.get();

        if (!entity->getMeshID().isNull())
            usedAssets.insert(entity->getMeshID());

        for (auto& [matIdx, matUUID] : entity->getMaterialIndexUUIDs()) {
            if (matUUID != UUID::Null)
                usedAssets.insert(matUUID);
        }
        
    }

    // Serialize assets from the global assetRegistry
    for (const UUID& uuid : usedAssets) {
        auto it = assetRegistry.find(uuid);
        if (it == assetRegistry.end())
            continue;

        const AssetRecord& record = it->second;

        jRegistry["assets"].push_back({
            { "uuid", record.uuid.toUint64() },
            { "type", AssetTypeToString(record.type) },
            { "path", record.path.string() }
            });
    }

    // Save file
    std::ofstream out(registryPath);
    out << jRegistry.dump(4);
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
    namespace fs = std::filesystem;

    fs::path dir(folderPath);
    if (!fs::exists(dir))
        fs::create_directories(dir);

    std::string sceneName = scene.getName();
    fs::path finalPath = dir / (sceneName + ".json");

    json jScene;
    jScene["uuid"] = scene.getUUID().toUint64();
    jScene["name"] = sceneName;
    
    json jGlobalLighting;
    jGlobalLighting["ambient"] = {
        scene.getAmbientLighting().x,
        scene.getAmbientLighting().y,
        scene.getAmbientLighting().z
    };
    jScene["GlobalLighting"] = jGlobalLighting;

    jScene["entities"] = json::array();

    const auto& entities = scene.getEntities();

    for (const auto& entityPtr : entities)
    {
        Entity* entity = entityPtr.get();
        json jEntity;

        jEntity["entityID"] = entity->getID().toUint64();
        jEntity["name"] = entity->getName();
        jEntity["type"] = entity->getType();
        jEntity["meshID"] = entity->getMeshID().toUint64();

        const auto& transform = entity->getTransform();
        jEntity["transform"] = {
            { "position", { transform.position.x, transform.position.y, transform.position.z } },
            { "rotation", { transform.rotation.x, transform.rotation.y, transform.rotation.z } },
            { "scale",    { transform.scale.x,    transform.scale.y,    transform.scale.z } }
        };

        // ---------- SAVE MATERIALS (NEW) ----------
        const auto& matUUIDs = entity->getMaterialIndexUUIDs();
        if (!matUUIDs.empty())
        {
            json jMaterials = json::object();

            for (const auto& [matIndex, matUUID] : matUUIDs)
            {
                jMaterials[std::to_string(matIndex)] = matUUID.toUint64();
            }

            jEntity["materials"] = jMaterials;
        }
        // ------------------------------------------

        // ---------- SAVE LIGHT ----------
        if (entity->hasLight())
        {
            const Light& light = entity->getLight();
            json jLight;

            jLight["type"] = static_cast<int>(light.type);

            jLight["position"] = { light.position.x,  light.position.y,  light.position.z };
            jLight["direction"] = { light.direction.x, light.direction.y, light.direction.z };

            jLight["ambient"] = { light.ambient.x,  light.ambient.y,  light.ambient.z };
            jLight["diffuse"] = { light.diffuse.x,  light.diffuse.y,  light.diffuse.z };
            jLight["specular"] = { light.specular.x, light.specular.y, light.specular.z };

            jLight["attenuation"] = {
                { "constant",  light.constant  },
                { "linear",    light.linear    },
                { "quadratic", light.quadratic }
            };

            jLight["spot"] = {
                { "cutOff",      light.cutOffAngle      },
                { "outerCutOff", light.outerCutOffAngle }
            };

            jEntity["light"] = jLight;
        }
        // ------------------------------------------

        jScene["entities"].push_back(jEntity);
    }

    std::ofstream file(finalPath);
    file << jScene.dump(4);

    std::cout << "Saved scene \"" << sceneName
        << "\" at: " << finalPath << std::endl;
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
        file >> jScene;   
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

    try {
        if (jScene.contains("GlobalLighting")) {
            auto jGlobalLighting = jScene.at("GlobalLighting");

            if (jGlobalLighting.contains("ambient")) {
                auto g_ambient = jGlobalLighting.at("ambient");
                scene->setAmbientLighting({
                    g_ambient[0].get<float>(),
                    g_ambient[1].get<float>(),
                    g_ambient[2].get<float>()
                    });
            }
        }

    }
    catch(const json::exception& e){
        std::cerr << "Invalid Global Lightning : " << sceneName << std::endl;
    }

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
                
                if (jEntity.contains("light"))
                {
                    const json& jLight = jEntity.at("light");

                    entity->addLight();
                    Light& light = entity->getLight();

                    light.type = static_cast<LightType>(
                        jLight.at("type").get<int>()
                        );

                    if (jLight.contains("direction")) {
                        auto d = jLight.at("direction");
                        light.direction = glm::normalize(glm::vec3(d[0], d[1], d[2]));
                    }

                    auto amb = jLight.at("ambient");
                    auto diff = jLight.at("diffuse");
                    auto spec = jLight.at("specular");

                    light.ambient = { amb[0],  amb[1],  amb[2] };
                    light.diffuse = { diff[0], diff[1], diff[2] };
                    light.specular = { spec[0], spec[1], spec[2] };

                    if (jLight.contains("attenuation"))
                    {
                        const auto& att = jLight.at("attenuation");
                        light.constant = att.at("constant").get<float>();
                        light.linear = att.at("linear").get<float>();
                        light.quadratic = att.at("quadratic").get<float>();
                    }

                    if (light.type == LightType::Spotlight && jLight.contains("spot"))
                    {
                        const auto& spot = jLight.at("spot");
                        light.cutOffAngle = spot.at("cutOff").get<float>();
                        light.outerCutOffAngle = spot.at("outerCutOff").get<float>();
                    }
                }


                Mesh* mesh = getMesh(meshUUID);

                if (!mesh)
                {
                    const AssetRecord* record = getRecord(meshUUID);
                    if (record)
                    {
                        loadAssetFromPath(*record);
                        mesh = getMesh(meshUUID); // retry
                    }
                }

                if (mesh)
                {
                    if (jEntity.contains("materials"))
                    {
                        const json& jMaterials = jEntity.at("materials");

                        for (auto it = jMaterials.begin(); it != jMaterials.end(); ++it)
                        {
                            unsigned int matIndex = std::stoul(it.key());
                            UUID matUUID(it.value().get<uint64_t>());

                            entity->getMaterialIndexUUIDs()[matIndex] = matUUID;

                            if (!getMaterial(matUUID)) {
                                const AssetRecord* matRecord = getRecord(matUUID);
                                if (matRecord)
                                {
                                    loadAssetFromPath(*matRecord);
                                    mesh = getMesh(matUUID); // retry
                                }

                            }
                        }
                    }

                    scene->assignDefaultMaterials(entity, mesh);
                }
                else
                {
                    std::cerr << "Mesh missing: UUID(" << meshUUID <<")"<< std::endl;
                }

            }
            catch (const json::exception& e) {
                std::cerr << "Skipping invalid entity in scene \"" << sceneName
                    << "\": " << e.what() << std::endl;
                continue; 
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
