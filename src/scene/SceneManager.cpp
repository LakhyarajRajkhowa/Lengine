#include "SceneManager.h"

using namespace Lengine;

void SceneManager::setActiveScene(Scene* scene) {
	if (!scene)
		return;
	
	activeScene = scene;
}

void SceneManager::loadScenes(const std::vector<std::string>& scenesToBeLoaded) {
	for (auto& scene : scenesToBeLoaded) {
		scenes.insert(assetManager.loadScene(Paths::GameScenes + scene + ".json"));
	}

	activeScene = *scenes.begin();

}