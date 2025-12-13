#pragma once
#include <unordered_map>
#include <memory>
#include <filesystem>
#include "./core/settings.h"
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../graphics/geometry/Mesh.h"
#include "../graphics/geometry/Model.h"
#include "../resources/TextureCache.h"
#include "../utils/metaFileSystem.h"
#include "../utils/modelFileSystem.h"
#include "../scene/Scene.h"
namespace fs = std::filesystem;

namespace Lengine {

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
			//LoadAllMetaFiles("../assets");
		}
		void LoadAllMetaFiles(const fs::path& root);

		void loadMesh(const UUID& uuid, const std::string& path);
		UUID getMeshUUID(const std::string& name);
		UUID importMesh(const std::string& path);
		UUID importAndLoadMesh(const std::string& name, const std::string& path);
		Mesh* getMesh(const UUID& id);

		void loadMaterial(
			const UUID& uuid,
			const std::string& path,
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath
		);
		UUID getMaterialUUID(const std::string& name);
		UUID importMaterial(
			const std::string& path);
		UUID importAndLoadMaterial(
			const std::string& name,
			const std::string& path,
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath
		);
		Material* getMaterial(const UUID& id);

		void saveScene(const Scene& scene, const std::string& filePath);
		Scene* createScene(const std::string& name,  const std::string& folderPath);
		Scene* AssetManager::loadScene(const std::string& filePath);

		void saveModelFile(const UUID& meshUUID);

		GLTexture* getTexture(const std::string& name);
		GLTexture* loadTexture(const std::string& name, const std::string& path);

		void linkMaterials(Entity* entity);

        GLSLProgram* loadShader(const std::string& name,
            const std::string& vertPath,
            const std::string& fragPath);
		GLSLProgram* getShader(const std::string& name);
	private:
		EngineSettings& settings;
		std::string modelFolderPath;
		std::string metaFolderPath;
	};
}