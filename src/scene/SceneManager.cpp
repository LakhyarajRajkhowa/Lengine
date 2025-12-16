#include "SceneManager.h"

using namespace Lengine;

void SceneManager::setActiveScene(Scene* scene) {
	if (!scene)
		return;
	for (auto& entity : scene->getEntities()) {
		
		// check if materialIds are indexed
		if (!entity->getMeshID()) {
			entity->setMeshID(UUID());
		}
		else {
			UUID meshID = entity->getMeshID();
			if (!assetManager.getMesh(meshID)) {
				continue;
			}
			else {
				if (!entity->getMaterialIndexUUIDs().empty()) {
					continue;
				}
				else {
					Mesh* mesh = assetManager.getMesh(meshID);
					scene->assignDefaultMaterials(entity.get(), mesh);
				}
			}
		}	
	}
	activeScene = scene;
}