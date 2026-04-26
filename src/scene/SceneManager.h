#pragma once
#include "../scene/Scene.h"
#include "../resources/AssetManager.h"

namespace Lengine {

    class SceneManager {
    private:
        std::unordered_set<Scene*> scenes;
        Scene* activeScene;

        std::unique_ptr<Scene> runtimeScene = nullptr;
        AssetManager& assetManager;
    public:
        SceneManager(AssetManager& asstMgr) : assetManager(asstMgr) {}   
        Scene* GetActiveScene(const EditorMode mode) { 
            if (mode == EditorMode::EDIT)
                return activeScene;
            else
                return runtimeScene.get();
        }

        Scene* GetEditorScene() { return activeScene; }
        std::unique_ptr<Scene>& GetRuntimeScene() { return runtimeScene; }

        void CreateRuntimeScene() { runtimeScene = activeScene->Clone(); }
        void setActiveScene(Scene* scene);

        const std::unordered_set<Scene*>& getScenes() const { return scenes; }
        std::unordered_set<Scene*>& getScenes() { return scenes; }

        void loadScenes(const std::vector<std::string>& scenes);
        
    };
} 