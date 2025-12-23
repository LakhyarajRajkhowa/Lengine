#pragma once
#include <unordered_map>
#include <memory>
#include <filesystem>
#include "../core/settings.h"
#include "../core/paths.h"
#include "../assets/AssetRegistry.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../graphics/geometry/Mesh.h"
#include "../graphics/geometry/Model.h"
#include "../graphics/material/MTLLoader.h"
#include "../assets/MaterialRegistry.h"
#include "../resources/TextureCache.h"
#include "../utils/metaFileSystem.h"
#include "../utils/modelFileSystem.h"
#include "../scene/Scene.h"
namespace fs = std::filesystem;

namespace Lengine {
	enum class AssetState {
		Failed = 0,
		Loading = 1,
		LoadedToCPU = 2,
		LoadedToGPU = 4,
		Loaded = 5,
		Unloaded = 6,
		
	};

	struct Asset {
		AssetType type;
		AssetState state;
		UUID id;
	};


	class AssetManager {
	private:
		std::unordered_map<UUID, std::shared_ptr<Mesh>> meshes;
		std::unordered_map<std::string, std::shared_ptr<GLSLProgram>> shaders;
		std::unordered_map<UUID, std::shared_ptr<GLTexture>> textures;
		std::unordered_map<UUID, std::shared_ptr<Material>> materials;

		TextureCache textureCache;
		
	public:
		AssetManager::AssetManager(EngineSettings& set):
			settings(set)
		{
			
		}
		void LoadAllMetaFiles(const fs::path& root);

		// MESH
		void requestMeshLoad(const UUID& uuid, const std::string& path);
		void loadMesh(const UUID& uuid, const std::string& path);
		UUID getMeshUUID(const std::string& name);
		UUID importMesh(const std::string& path);
		UUID importAndLoadMesh(const std::string& name, const std::string& path);
		Mesh* getMesh(const UUID& id);

		// MATERIRALS
		bool loadMaterial(
			const UUID& uuid,
			const std::string& path,
			const std::string& vertexShaderPath = ShaderPath::defaultVertexShaderPath,
			const std::string& fragmentShaderPath = ShaderPath::defaultFragmentShaderPath
		);
		UUID getMaterialUUID(const std::string& name);
		UUID importMaterial(
			const std::string& path);
		UUID importAndLoadMaterial(
			const std::string& name,
			const std::string& path,
			const std::string& vertexShaderPath = ShaderPath::defaultVertexShaderPath,
			const std::string& fragmentShaderPath =  ShaderPath::defaultFragmentShaderPath
		);
		Material* getMaterial(const UUID& id);

		// SCENE
		void saveScene(const Scene& scene, const std::string& filePath);
		Scene* createScene(const std::string& name,  const std::string& folderPath);
		Scene* AssetManager::loadScene(const std::string& filePath);

		void saveModelFile(const UUID& meshUUID);

		// TEXTURES
		UUID importTexture(const std::string& path);
		GLTexture* getTexture(const UUID& id);
		void requestTextureLoad(const UUID& uuid, const std::string& path);
		void loadTexture(const UUID& uuid, const std::string& path);

		GLTexture* loadImage(const std::string& name, const std::string& path);
		
		// SHADERS
        GLSLProgram* loadShader(const std::string& name,
            const std::string& vertPath,
            const std::string& fragPath);
		GLSLProgram* getShader(const std::string& name);

		// ASSET REGSISTRY
		const AssetRecord* getRecord(const UUID& uuid) const;
		bool loadAssetFromPath(const AssetRecord& record);
		bool loadAssetRegistry(const std::string& filePath);
		void AssetManager::addAsset(
			const UUID& uuid,
			AssetType type,
			const std::string& path
		);
		void saveSceneAssetRegistryForScene(
			const Scene& scene,
			const std::string& folderPath
		);
		std::unordered_map<UUID, AssetState> assetStates;
		std::mutex assetMutex;

		void processGpuUploads();
		void syncAssetsToScene( Scene& activeScene);
		bool hasLoadingAssets();
		float getLoadingProgress(const UUID& id) const;

		UUID currentLoadingAsset = UUID::Null;
		UUID getCurrentLoadingAsset() { return currentLoadingAsset; }
		void drawLoadingScreens();
		AssetType getAssetType(const UUID& id);

	private:
		EngineSettings& settings;
		std::string modelFolderPath;
		std::string metaFolderPath;
	};
}