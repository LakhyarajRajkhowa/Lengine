#include "AssetManager.h"

using namespace Lengine;

void AssetManager::Init() {
    AssetDatabase::LoadDatabase();
    LoadAllDefaultAssets();
}

void AssetManager::LoadAllDefaultAssets() {

    for (auto id : SubmeshID::GetAllDefaults()) {
        LoadSubmesh(id);
    }

    for (auto id : MaterialID::GetAllDefaults()) {
        LoadMaterial(id);
    }

    for (auto shader : ShaderRegistry::GetAllDefaults()) {
        loadShader(shader.name, shader.vertexShaderPath, shader.fragemnetShaderPath);
    }

    for (auto asset : AssetDatabase::GetAllAssets()) {
        if (asset.second.type == AssetType::Texture) {
            texturePathToUUID[asset.second.sourcePath.string()] = asset.second.uuid;
        }
    }
}

void AssetManager::Update(Scene& activeScene) {
    UpdateAllAssetViews();
  //  UpdateAssetStates();
    SyncAssetsToScene(activeScene);
    ProcessGpuUploads();


}

void AssetManager::UpdateAllAssetViews()
{
    if (!AssetDatabase::needsUpdate)
        return;

    submeshViews.clear();
    pbrMaterialViews.clear();
    TextureViews.clear();
    PrefabViews.clear();

    const auto& allAssets = AssetDatabase::GetAllAssets();

    for (const auto& [uuid, meta] : allAssets)
    {
        AssetView view;
        view.uuid = uuid;
        view.name = meta.name;
        view.type = meta.type;
        view.libraryPath = meta.libraryPath;
        view.thumbnailPath = meta.thumbnailPath;

        switch (meta.type)
        {
        case AssetType::Submesh:
            submeshViews.emplace_back(view);
            break;

        case AssetType::Material:
            pbrMaterialViews.emplace_back(view);
            break;

        case AssetType::Texture:
            TextureViews.emplace_back(view);
            break;
        case AssetType::Prefab:
            PrefabViews.emplace_back(view);
            break;

        default:
            break;
        }
    }

    AssetDatabase::needsUpdate = false;
}

    



//  ---- SUBMESH ---

void AssetManager::RequestSubmeshLoad(const UUID& submeshID, const UUID& entityID)
{
    if (GetSubmesh(submeshID)) {
        pendingSubmeshes.push({ entityID, submeshID });
        assetStates[submeshID] = AssetState::LoadedToGPU;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[submeshID] = AssetState::Loading;
        pendingSubmeshes.push({ entityID, submeshID });
    }

    std::thread([this, submeshID]()
        {
            bool ok = LoadSubmesh(submeshID);

            std::lock_guard<std::mutex> lock(assetMutex);
            assetStates[submeshID] = ok
                ? AssetState::LoadedToCPU
                : AssetState::Failed;
        }).detach();
}


bool AssetManager::LoadSubmesh(const UUID& id) {
    auto sm = AssetDatabase::LoadAsset<Submesh>(id);

    if (sm)
        submeshes[id] = sm;
    else
        return false;

    return true;
}

bool AssetManager::processPendingSubmesh(const UUID& id) {
    
    auto& submesh = submeshes[id];
    if (submesh) {
        submesh->setupMesh();
        return true;
    }

    return false;

        
}

Submesh* AssetManager::GetSubmesh(const UUID& id) {
    auto it = submeshes.find(id);
    if (it == submeshes.end())
        return nullptr;

    return it->second.get();
}

// -------- SKELETON ----------

bool AssetManager::LoadSkeleton(const UUID& uuid) {
    auto skeleton = AssetDatabase::LoadAsset<Skeleton>(uuid);

    if (skeleton)
        skeletons[uuid] = skeleton;
    else
        return false;

    return true;
}

Skeleton* AssetManager::GetSkeleton(const UUID& id) {
    auto it = skeletons.find(id);
    if (it == skeletons.end())
        return nullptr;

    return it->second.get();
}

// -------- ANIMATION ----------

bool AssetManager::LoadAnimation(const UUID& uuid) {
    auto anim = AssetDatabase::LoadAsset<Animation>(uuid);

    if (anim)
        animations[uuid] = anim;
    else
        return false;

    return true;
}

Animation* AssetManager::GetAnimation(const UUID& id) {
    auto it = animations.find(id);
    if (it == animations.end())
        return nullptr;

    return it->second.get();
}


// -------- PBRMATERIAL ---------

void AssetManager::ImportMaterial(const std::string path) {
    const UUID& uuid = UUID();
    AssetImporter::ImportMaterialFile(path, uuid);
}

UUID AssetManager::CreateMaterial(const std::string name) {
    return MaterialCreator::Create(name);
}

void AssetManager::SaveMaterial(const UUID& id) {
    const Material& mat = *GetMaterial(id);
    const std::filesystem::path libPath = GetAssetMetaData(id)->libraryPath;

    MaterialSaver::Save(mat, libPath);
}

bool AssetManager::LoadMaterial(const UUID& matID) {

    if (GetMaterial(matID)) return true;

    auto mat = AssetDatabase::LoadAsset<Material>(matID);

    if (mat) {

        mat->id = matID;
        pbrMaterials[matID] = mat;

        // albedo
        if (!mat->map_albedo_path.empty()) {

            UUID texID = GetOrCreateTextureUUID(mat->map_albedo_path);

            if (haveAssetState(texID))
            {
                mat->map_albedo = texID;
            }
            else
            {
                ImportTexture(mat->map_albedo_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::Albedo,
                    TextureTargetType::GlobalMaterial,
                    true
                    });
            }
        }

        // normal
        if (!mat->map_normal_path.empty()) {
            UUID texID = GetOrCreateTextureUUID(mat->map_normal_path);

            if (haveAssetState(texID))
            {
                mat->map_normal = texID;
            }
            else
            {
                ImportTexture(mat->map_normal_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::Normal,
                    TextureTargetType::GlobalMaterial,
                    false
                    });
            }

        }

        // metallic
        if (!mat->map_metallic_path.empty()) {
            UUID texID = GetOrCreateTextureUUID(mat->map_metallic_path);

            if (haveAssetState(texID))
            {
                mat->map_metallic = texID;
            }
            else
            {
                ImportTexture(mat->map_metallic_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::Metallic,
                    TextureTargetType::GlobalMaterial,
                    false
                    });
            }
        }

        // roughness
        if (!mat->map_roughness_path.empty()) {

            UUID texID = GetOrCreateTextureUUID(mat->map_roughness_path);

            if (haveAssetState(texID))
            {
                mat->map_roughness = texID;
            }
            else
            {
                ImportTexture(mat->map_roughness_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::Roughness,
                    TextureTargetType::GlobalMaterial,
                    false
                    });
            }
        }

        // ambient occulision
        if (!mat->map_ao_path.empty()) {
            UUID texID = GetOrCreateTextureUUID(mat->map_ao_path);

            if (haveAssetState(texID))
            {
                mat->map_ao = texID;
            }
            else
            {
                ImportTexture(mat->map_ao_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::AmbientOcclusion,
                    TextureTargetType::GlobalMaterial,
                    false
                    });
            }
        }

        // arm
        if (!mat->map_metallicRoughness_path.empty()) {
            UUID texID = GetOrCreateTextureUUID(mat->map_metallicRoughness_path);

            if (haveAssetState(texID))
            {
                mat->map_metallicRoughness = texID;
            }
            else
            {
                ImportTexture(mat->map_metallicRoughness_path, texID);

                std::lock_guard<std::mutex> lock(assetMutex);
                pendingTextureRequests.push({
                    texID,
                    matID,
                    TextureMapType::MetallicRoughness,
                    TextureTargetType::GlobalMaterial,
                    false
                    });
            }
        }

        return true;

    }
        
    else
        return false;
}




const AssetMetadata* AssetManager::GetAssetMetaData(const UUID& uuid) const
{
    const auto& allAssets = AssetDatabase::GetAllAssets();

    auto it = allAssets.find(uuid);
    if (it == allAssets.end())
        return nullptr;

    return &it->second;
}



//    ----- MESH ----
void AssetManager::ImportPrefab(const std::string& path) {
    UUID assetID = UUID();
    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = AssetState::Importing;
        currentImportingAsset.second = path;
    }
  
    std::thread([this, path, assetID]() {

        bool ok = AssetImporter::ImportPrefabFile(path, assetID);

        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = ok ? AssetState::Imported
            : AssetState::Failed;

        }).detach();


}

bool AssetManager::LoadPrefabToScene(const std::string& path) {
    
    PrefabData data = LoadPrefabFile(path);

    pendingPrefabs.push(data);

    return true;
}


Entity* AssetManager::InstantiatePrefab(
    Scene& scene,
    const PrefabData& prefab
)
{
    std::vector<Entity*> entities(prefab.nodes.size(), nullptr);

    // ---- Create entities ----
    for (const auto& node : prefab.nodes)
    {
        Entity* e = scene.createEntity(node.name);
        entities[node.index] = e;

        auto& t = scene.Transforms().Add(e->getID());

        TransformSystem::DecomposeMatrix(
            node.localTransform,
            t.localPosition,
            t.localRotation,
            t.localScale
        );

        t.localDirty = true;
        t.worldDirty = true;

        TransformSystem::Dirty = true;

        if (node.meshID != UUID::Null)
        {

            // Submesh
            if (!scene.MeshFilters().Has(e->getID())) {
                auto& mf = scene.MeshFilters().Add(e->getID());
                RequestSubmeshLoad(node.meshID, e->getID());

            }
            else {
                RequestSubmeshLoad(node.meshID, e->getID());
            }

            // Material
            if (node.materialID != UUID::Null)
            {
                
                if (!scene.MeshRenderers().Has(e->getID())) {
                    auto& mr = scene.MeshRenderers().Add(e->getID());
                    
                    
                    if (LoadMaterial(node.materialID)) {
                        mr.inst.baseMaterial = node.materialID;
                        mr.inst.dirty = true;
                    }
                }
                else {
                    auto& mr = scene.MeshRenderers().Get(e->getID());

                    if (LoadMaterial(node.materialID)) {
                        mr.inst.baseMaterial = node.materialID;
                        mr.inst.dirty = true;

                    }
                }


            }
            else {
                if (!scene.MeshRenderers().Has(e->getID())) {
                    auto& mr = scene.MeshRenderers().Add(e->getID());
                    mr.inst.baseMaterial = MaterialID::DefaultPbr;

                }
                else {
                    auto& mr = scene.MeshRenderers().Get(e->getID());
                    mr.inst.baseMaterial = MaterialID::DefaultPbr;

                }
            }

        }  
    }

    for (const auto& node : prefab.nodes)
    {
        if (node.parentIndex != -1)
        {
            scene.SetParent(
                entities[node.index]->getID(),
                entities[node.parentIndex]->getID()
            );
        }
    }

    for (auto& e : entities) {
        if (scene.MeshFilters().Has(e->getID())) {
            auto& m = scene.MeshFilters().Get(e->getID());
            m.rootParent = entities[0]->getID();
        }
    }

    // -------- SKELETON (root) --------
    if (prefab.skeletonID != UUID::Null)
    {
        if (!scene.Skeletons().Has(entities[0]->getID()))
        {
            auto& sk = scene.Skeletons().Add(entities[0]->getID());

            sk.skeletonID = prefab.skeletonID;

            // load skeleton asset
            if (GetSkeleton(sk.skeletonID)) {
                sk.skeleton = GetSkeleton(sk.skeletonID);
            }
            else {
                if (LoadSkeleton(sk.skeletonID)) {
                    sk.skeleton = GetSkeleton(sk.skeletonID);
                }

            }
        }

    }

    // -------- ANIMATION (root) --------
    if (!prefab.animationIDs.empty())
    {
        if (!scene.Animations().Has(entities[0]->getID()))
        {
            auto& anim = scene.Animations().Add(entities[0]->getID());

            anim.animationIDs = prefab.animationIDs;


            for (auto& animID : anim.animationIDs) {
                if (!GetAnimation(animID)) {
                    LoadAnimation(animID);
                }
            }

            UUID firstAnimID = prefab.animationIDs[0];
            if (GetAnimation(firstAnimID))
            {
                anim.currentAnimationID = firstAnimID;
            }

            anim.currentTime = 0.0f;
            anim.playbackSpeed = 1.0f;
            anim.looping = true;
        }
    }


    return entities[0];
}





void AssetManager::ImportMesh(const std::string& path) {

    UUID assetID = UUID();

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = AssetState::Importing;
        currentImportingAsset.second = path;

    }
 

    std::thread([this, path, assetID]() {

        bool ok = AssetImporter::ImportMeshFile(path, assetID);

        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = ok ? AssetState::Imported
            : AssetState::Failed;

    }).detach();
}






Material* AssetManager::GetMaterial(const UUID& id)
{
    auto it = pbrMaterials.find(id);
    if (it == pbrMaterials.end())
        return nullptr;

    return it->second.get();
}



//          ----- TEXTURES -----

void AssetManager::ImportTexture(const std::string& path, const UUID& assetID) {

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = AssetState::Importing;
        currentImportingAsset.second = path;

    }


    std::thread([this, path, assetID]() {

        bool ok = AssetImporter::ImportTextureFile(path, assetID);

        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[assetID] = ok ? AssetState::Imported
            : AssetState::Failed;

        }).detach();
}

UUID AssetManager::GetOrCreateTextureUUID(const std::string& path)
{
    std::lock_guard<std::mutex> lock(assetMutex);

    auto it = texturePathToUUID.find(path);
    if (it != texturePathToUUID.end())
        return it->second;

    UUID newID = UUID();
    texturePathToUUID[path] = newID;
    return newID;
}

// this one for only Global Material Asset
void AssetManager::RequestTextureLoad(
    const UUID& texID,
    const UUID& matID,
    const TextureMapType& mapType,
    bool srgb
)
{
    if (getTexture(texID)) {
        pendingTextureRequests.push({
           texID,
           matID,
           mapType,
           TextureTargetType::GlobalMaterial
            });
        assetStates[texID] = AssetState::LoadedToGPU;
        return;
    } 

    auto tex = std::make_shared<GLTexture>();
    tex->srgb = srgb;

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[texID] = AssetState::Loading;

        pendingTextureRequests.push({
            texID,
            matID,
            mapType,
            TextureTargetType::GlobalMaterial
            });
    }

    std::thread([this, texID, tex]()
        {
            auto img = LoadTexture(texID);
            if (!img) return;

            tex->imageCPU = std::move(img);
            tex->pendingGPUUpload = true;
            textures[texID] = tex;

            std::lock_guard<std::mutex> lock(assetMutex);
            assetStates[texID] = AssetState::LoadedToCPU;
        }).detach();
}


// this one for inst material per meshrenderer
void AssetManager::RequestTextureLoad_inst(
    const UUID& texID,
    const UUID& entityID,
    const TextureMapType& mapType,
    bool srgb
)
{
    if (getTexture(texID)) {

        pendingTextureRequests.push({
            texID,
            entityID,
            mapType,
            TextureTargetType::MeshRendererInstance
            });
        assetStates[texID] = AssetState::LoadedToGPU;
        return;
    }

    auto tex = std::make_shared<GLTexture>();
    tex->srgb = srgb;

    {
        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[texID] = AssetState::Loading;

        pendingTextureRequests.push({
            texID,
            entityID,
            mapType,
            TextureTargetType::MeshRendererInstance
            });
    }

    std::thread([this, texID, tex]()
        {
            auto img = LoadTexture(texID);
            if (!img) {
                textures.erase(texID);
                return;
            }

            tex->imageCPU = std::move(img);
            tex->pendingGPUUpload = true;
            textures[texID] = tex;

            std::lock_guard<std::mutex> lock(assetMutex);
            assetStates[texID] = AssetState::LoadedToCPU;
        }).detach();
}


std::shared_ptr<ImageData> AssetManager::LoadTexture(const UUID& id) {
    std::shared_ptr<ImageData> img = AssetDatabase::LoadAsset<ImageData>(id);

    if (!img) {
        std::lock_guard<std::mutex> lock(assetMutex);

        assetStates[id] = AssetState::Failed;
        return nullptr;
    }


    return img;
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
    *tex = textureCache.getTexture(path, false); // sRGB = false
    textures[id] = tex;
    return tex.get();

}

bool AssetManager::processPendingTextures(const UUID& uuid)
{
    GLTexture* tex = getTexture(uuid);
    if (!tex || !tex->pendingGPUUpload || !tex->imageCPU)
        return false;


    GLenum format;
    switch (tex->imageCPU->channels)
    {
    case 1: format = GL_RED;  break;
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    default:
        assetStates[uuid] = AssetState::Failed;
        return false;
    }

    GLenum internalFormat;
    if (tex->srgb)
    {
        if (format == GL_RGBA) internalFormat = GL_SRGB8_ALPHA8;
        else if (format == GL_RGB) internalFormat = GL_SRGB8;
        else internalFormat = GL_R8;
    }
    else
    {
        if (format == GL_RGBA) internalFormat = GL_RGBA8;
        else if (format == GL_RGB) internalFormat = GL_RGB8;
        else internalFormat = GL_R8;
    }

    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    glTexImage2D(
        GL_TEXTURE_2D, 0, internalFormat,
        tex->imageCPU->width, tex->imageCPU->height,
        0, format, GL_UNSIGNED_BYTE,
        tex->imageCPU->pixels.data()
    );

    if (glGetError() != GL_NO_ERROR)
    {
        glDeleteTextures(1, &tex->id);
        tex->id = 0;
        assetStates[uuid] = AssetState::Failed;
        return false;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    tex->imageCPU.reset();
    tex->pendingGPUUpload = false;

    return true;
}


GLTexture* AssetManager::getTexture(const UUID& id) {
    return textures[id].get();
}


//          ----- SHADERS -----
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


//          ----- ASSET REGISTRY -----



// ASSET DATABASE

void AssetManager::LoadAssetDatabase() {
    AssetDatabase::LoadDatabase();
}

void AssetManager::saveAssetDatabase() {
    AssetDatabase::SaveDatabase();
}


//          ----- SCENE -----

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

    jScene["entities"] = json::array();

    const auto& entities = scene.getEntities();

    for (const auto& entityPtr : entities)
    {
        const UUID entityID = entityPtr.get()->getID();
        const std::string entityName = scene.NameTags().Get(entityID).name;


        json jEntity;

        jEntity["entityID"] = entityID.toUint64();
        jEntity["name"] = entityName;

        if (scene.Transforms().Has(entityID)) {
            const TransformComponent& entityTransform = scene.Transforms().Get(entityID);

            jEntity["transform"] = {
                { "position", { entityTransform.localPosition.x, entityTransform.localPosition.y, entityTransform.localPosition.z } },
                { "rotation", { entityTransform.localRotation.x, entityTransform.localRotation.y, entityTransform.localRotation.z, entityTransform.localRotation.w } },
                { "scale",    { entityTransform.localScale.x,    entityTransform.localScale.y,    entityTransform.localScale.z } }
            };
        }
       

        if (scene.MeshFilters().Has(entityID)) {
            const MeshFilter& mf = scene.MeshFilters().Get(entityID);

            json jMeshFilter;

            jMeshFilter["meshID"] = mf.submeshID.toUint64();

            jEntity["meshFilter"] = jMeshFilter;

        }

        if (scene.MeshRenderers().Has(entityID)) {
            const MeshRenderer& mr = scene.MeshRenderers().Get(entityID);

            json jMeshRenderer;

            jMeshRenderer["materialID"] = mr.inst.baseMaterial.toUint64();

            jEntity["meshRenderer"] = jMeshRenderer;
        }

        if (scene.Lights().Has(entityID)) {
            const Light& l = scene.Lights().Get(entityID);

            json jLight;

            
            if (scene.Transforms().Has(entityID)) {
                const TransformComponent& entityTransform = scene.Transforms().Get(entityID);
                jLight["position"] = { entityTransform.localPosition.x,  entityTransform.localPosition.y,  entityTransform.localPosition.z };
            }

            jLight["color"] = { l.color.x, l.color.y, l.color.z };

            jEntity["light"] = jLight;
        }


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

    try
    {
        fs::path path(filePath);

        if (!fs::exists(path))
        {
            std::cerr << "Scene file does not exist: " << filePath << "\n";
            return nullptr;
        }

        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open scene file: " << filePath << "\n";
            return nullptr;
        }

        json jScene;
        file >> jScene;
        file.close();

      
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

        // ---- Entities ----
        const auto& jEntities = jScene.at("entities");

        for (const auto& jEntity : jEntities)
        {
            try {
                UUID entityID = UUID(jEntity.at("entityID").get<uint64_t>());
                std::string entityName = jEntity.at("name").get<std::string>();

                scene->NameTags().Add(entityID, NameTagComponent(entityName));

                Entity* entity = scene->createEntity(entityName, entityID);

                // ---- Transform ----
                if (jEntity.contains("transform"))
                {
                    const auto& jt = jEntity.at("transform");

                    TransformComponent tr;

                    tr.localPosition = {
                        jt.at("position")[0],
                        jt.at("position")[1],
                        jt.at("position")[2]
                    };

                    tr.localRotation = glm::quat(
                        jt.at("rotation")[3],  // w
                        jt.at("rotation")[0],  // x
                        jt.at("rotation")[1],  // y
                        jt.at("rotation")[2]   // z
                    );

                    tr.localScale = {
                        jt.at("scale")[0],
                        jt.at("scale")[1],
                        jt.at("scale")[2]
                    };


                    scene->Transforms().Add(entityID, tr);
                }


                // ---- MeshFilter ----
                if (jEntity.contains("meshFilter"))
                {
                    MeshFilter mf;
                    mf.submeshID = UUID(jEntity["meshFilter"].at("meshID").get<uint64_t>());
                    scene->MeshFilters().Add(entityID, mf);
                }

                // ---- MeshRenderer ----
                if (jEntity.contains("meshRenderer"))
                {
                    MeshRenderer mr;
                    mr.inst.baseMaterial = UUID(jEntity["meshRenderer"].at("materialID").get<uint64_t>());
                    scene->MeshRenderers().Add(entityID, mr);
                }

                // ---- Light ----
                if (jEntity.contains("light"))
                {
                    const auto& jl = jEntity.at("light");

                    Light light;
                   

                    light.color = {
                        jl.at("color")[0],
                        jl.at("color")[1],
                        jl.at("color")[2]
                    };

                    scene->Lights().Add(entityID, light);
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

        std::cout << "Loaded scene: " << scene->getName() << "\n";
        return scene;
    }
    catch (const json::exception& e)
    {
        std::cerr << "JSON error while loading scene: " << e.what() << "\n";
        return nullptr;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while loading scene: " << e.what() << "\n";
        return nullptr;
    }
}


void AssetManager::ProcessGpuUploads()
{
    std::vector<UUID> gpuReadyAssets;

    {
        std::lock_guard<std::mutex> lock(assetMutex);

        for (auto& [id, state] : assetStates)
        {
            if (state == AssetState::LoadedToCPU)
                gpuReadyAssets.push_back(id);
        }
    }

    // GPU work OUTSIDE the lock
    for (UUID id : gpuReadyAssets)
    {
        AssetType type = getAssetType(id);
        bool ok = false;

        switch (type)
        {
        case AssetType::Submesh:
            ok = processPendingSubmesh(id);
            break;

        case AssetType::Texture:
            ok = processPendingTextures(id);
            break;
        }

        std::lock_guard<std::mutex> lock(assetMutex);
        assetStates[id] = ok
            ? AssetState::LoadedToGPU
            : AssetState::Failed;
    }
}




void AssetManager::SyncAssetsToScene(Scene& activeScene) {

    if (!pendingPrefabs.empty()) {
        InstantiatePrefab(activeScene, pendingPrefabs.front());

        pendingPrefabs.pop();
    }


    if (!pendingSubmeshes.empty())
    {
        auto req = pendingSubmeshes.front();

        if (assetStates[req.submeshID] == AssetState::LoadedToGPU)
        {
            auto& mf = activeScene.MeshFilters().Get(req.entityID);
            mf.submeshID = req.submeshID;
            mf.ClearPendingSubmesh();

            assetStates[req.submeshID] = AssetState::Loaded;
            pendingSubmeshes.pop();
        }
        else if (assetStates[req.submeshID] == AssetState::Failed)
        {
            pendingSubmeshes.pop();
        }
    }

    ProcessPendingTextureRequests(activeScene);

}

// this func is for clearing up the Failed or Loaded assets of assetStates 
// as processGpuUploads() iterarate through assetStates every loop
// it is better to keep only the assets which are needed for loading or importing
void AssetManager::UpdateAssetStates()
{
    // could use std::erase_if from C++20
    for (auto it = assetStates.begin(); it != assetStates.end(); )
    {
        AssetState state = it->second;

        if (
            state == AssetState::Failed ||
            state == AssetState::Unloaded
            )
        {
            it = assetStates.erase(it); 
        }
        else
        {
            ++it;
        }
    }
}



void AssetManager::ProcessPendingTextureRequests(Scene& activeScene)
{
    if (pendingTextureRequests.empty())
        return;

    size_t count = pendingTextureRequests.size();

    while (count--)
    {
        TextureLoadRequest req = pendingTextureRequests.front();
        pendingTextureRequests.pop();

        AssetState state = assetStates[req.textureID];

        if (state == AssetState::Importing 
            )
        {
            pendingTextureRequests.push(req);
            continue;
        }

        // 🔹 Stage 2: Imported but not loading → trigger load
        if (state == AssetState::Imported)
        {
            RequestTextureLoad(
                req.textureID,
                req.targetID,
                req.mapType,
                req.srgb
            );
            pendingTextureRequests.push(req);
            continue;
        }

        // 🔹 Stage 3: Waiting for GPU upload
        if (state != AssetState::LoadedToGPU)
        {
            pendingTextureRequests.push(req);
            continue;
        }

        // 🔹 Stage 4: Safe to bind
        if (req.targetType == TextureTargetType::GlobalMaterial)
        {
            Material* mat = GetMaterial(req.targetID);
            if (!mat) continue;

            switch (req.mapType)
            {
            case TextureMapType::Albedo: mat->map_albedo = req.textureID; break;
            case TextureMapType::Normal: mat->map_normal = req.textureID; break;
            case TextureMapType::Metallic: mat->map_metallic = req.textureID; break;
            case TextureMapType::Roughness: mat->map_roughness = req.textureID; break;
            case TextureMapType::AmbientOcclusion: mat->map_ao = req.textureID; break;
            case TextureMapType::MetallicRoughness:
                mat->map_metallicRoughness = req.textureID;
                break;
            }

            mat->localDirty = true;

            assetStates[req.textureID] = AssetState::Loaded;
        }
        else
        {
            if (!activeScene.MeshRenderers().Has(req.targetID))
                continue;

            MeshRenderer& mr =
                activeScene.MeshRenderers().Get(req.targetID);

            MaterialInstance& inst = mr.inst;

            switch (req.mapType)
            {
            case TextureMapType::Albedo: inst.map_albedo = req.textureID; break;
            case TextureMapType::Normal: inst.map_normal = req.textureID; break;
            case TextureMapType::Metallic: inst.map_metallic = req.textureID; break;
            case TextureMapType::Roughness: inst.map_roughness = req.textureID; break;
            case TextureMapType::AmbientOcclusion: inst.map_ao = req.textureID; break;
            case TextureMapType::MetallicRoughness:
                inst.map_metallicRoughness = req.textureID;
                break;
            }

            inst.dirty = true;

            assetStates[req.textureID] = AssetState::Loaded;
        }
    }
}

bool AssetManager::haveAssetState(const UUID& assetID)
{
    return assetStates.find(assetID) != assetStates.end();
}


bool AssetManager::hasLoadingAssets() 
{
    for (const auto& [id, state] : assetStates)
    {
        if (state == AssetState::Loading || state == AssetState::LoadedToCPU )
        { 
            currentLoadingAsset = id;
            return true;
        }
    }
    currentLoadingAsset = UUID::Null;
    return false;
}

bool AssetManager::hasImportingAssets()
{
    for (const auto& [id, state] : assetStates)
    {
        if (state == AssetState::Importing)
        {
            currentImportingAsset.first = id;
            return true;
        }
    }
    currentImportingAsset.first = UUID::Null;
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

float AssetManager::getImportingProgress(const UUID& id) const
{
    auto it = assetStates.find(id);
    if (it == assetStates.end())
        return -1.0f;

    AssetState state = it->second;
    switch (state) {
    case AssetState::Failed:
        return -1.0f;
    case AssetState::Importing:
        return 0.25f;
    case AssetState::Imported:
        return 1.0f;
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
        case AssetType::Submesh:
            assetPath = GetAssetMetaData(id)->libraryPath.string();
            config.title = "Loading Mesh";
            break;
        case AssetType::Texture:
            assetPath = GetAssetMetaData(id)->libraryPath.string();
            config.title = "Loading Texture";
            break;

        }

        config.message = "Loading :" + assetPath;
        LoadingSystem::progressBarUI(progess, config);
    }
}

void AssetManager::drawImportingScreens() {

    // Progress bar
    if (hasImportingAssets()) {
        UUID id = getCurrentImportingAsset().first;
        std::string assetPath = "Unknown";
        float progess = getImportingProgress(id);

        LoadingSystem::LoadingUIConfig config;

        assetPath = getCurrentImportingAsset().second;
        config.title = "Importing Asset";

        config.message = "Importing :" + assetPath;
        LoadingSystem::progressBarUI(progess, config);
    }
}

AssetType AssetManager::getAssetType(const UUID& id) {
    const AssetMetadata* meta = GetAssetMetaData(id);
    if (!meta) return AssetType::Unknown;

    if (meta->type == AssetType::Material) return AssetType::Material;
    else if (meta->type == AssetType::Texture) return AssetType::Texture;
    else if (meta->type == AssetType::Submesh) return AssetType::Submesh;
    else return AssetType::Unknown;
}




