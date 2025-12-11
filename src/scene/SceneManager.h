#pragma once
#include "../scene/Scene.h"

namespace Lengine {

    class SceneManager {
    private:
        std::unordered_set<Scene*> scenes;
        Scene* activeScene;
    public:
        Scene* getActiveScene() { return activeScene;  }
        void setActiveScene(Scene* scene) { activeScene = scene; }

        const std::unordered_set<Scene*>& getScenes() const { return scenes; }
        std::unordered_set<Scene*>& getScenes() { return scenes; }


    };
}