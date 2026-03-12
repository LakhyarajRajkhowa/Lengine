#pragma once
#include "../scene/Scene.h"
#include "../resources/AssetManager.h"

namespace Lengine {

    class SceneManager {
    private:
        std::unordered_set<Scene*> scenes;
        Scene* activeScene;

        AssetManager& assetManager;
    public:
        SceneManager(AssetManager& asstMgr) : assetManager(asstMgr) {}   
        Scene* getActiveScene() { return activeScene;  }
        void setActiveScene(Scene* scene);

        const std::unordered_set<Scene*>& getScenes() const { return scenes; }
        std::unordered_set<Scene*>& getScenes() { return scenes; }

        void loadScenes(const std::vector<std::string>& scenes);
        
    };
} 