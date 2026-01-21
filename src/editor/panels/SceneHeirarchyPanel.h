#pragma once
#include <imgui.h>
#include <vector>
#include <string>
#include <queue>
#include "../graphics/camera/Camera3d.h"
#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../resources/AssetManager.h"
#include "../external/tinyfiledialogs.h"


namespace Lengine {
    


    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel(
            Camera3d& camera,
            SceneManager& sceneManager,
            AssetManager& assetManager,
            Entity* selectedEntity
        );

        void OnImGuiRender();
        void createNewModel();
        void drawCreateScenePopup();
        void drawRenameScenePopup();
        Entity* getSelectedEntity() { return m_SelectedEntity; }
    private:
        Camera3d& camera;
        SceneManager& sceneManager;
        AssetManager& assetManager;
    private:
        Entity* m_SelectedEntity;
        std::queue<UUID> deletedEntityQueue;
        std::queue<std::pair<Entity*, UUID>> createdEntityQueue;

        Scene* activeScene;
    };

}
