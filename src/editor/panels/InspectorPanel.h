#pragma once
#include <imgui.h>

#include "AssetPanel.h"

#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
namespace Lengine {

    struct InspectorState {
        bool uniformScale = true;
    };


    class InspectorPanel {
    public:
        InspectorPanel(
            SceneManager& sceneManager,
            AssetManager& assetManager
        );

        void OnImGuiRender();
        void DrawEntityInspector(Entity* entity, AssetManager& assets);
    private:
        SceneManager& sceneManager;
        AssetManager& assetManager;

        Entity* selectedEntity = nullptr;
        InspectorState inspectorState;
    };

}
