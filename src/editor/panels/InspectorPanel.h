#pragma once
#include <imgui.h>
#include "AssetPanel.h"

#include "../scene/Scene.h"
#include "../graphics/renderer/Renderer.h"
namespace Lengine {

    class InspectorPanel {
    public:
        InspectorPanel(
            SceneManager& sceneManager,
            AssetManager& assetManager,
            Renderer& renderer
        );

        void OnImGuiRender();
        void DrawEntityInspector(Entity* entity, AssetManager& assets);
    private:
        SceneManager& sceneManager;
        AssetManager& assetManager;
        Renderer& renderer;
        Entity* selectedEntity = nullptr;

    };

}
