#include "SceneManager.h"

using namespace Lengine;

void SceneManager::setActiveScene(Scene* scene) {
	if (!scene)
		return;
	
	activeScene = scene;
}