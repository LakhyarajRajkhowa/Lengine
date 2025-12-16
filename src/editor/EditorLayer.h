#pragma once

#define IMGUI_ENABLE_DOCKING

#include "imgui.h"
#include "imgui_internal.h"

#include "../scene/Scene.h"
#include "../scene/SceneManager.h"

#include "../platform/Window.h"
#include "../editor/panels/ViewportPanel.h"
#include "../editor/panels/SceneHeirarchyPanel.h"
#include "../editor/panels/InspectorPanel.h"
#include "../editor/panels/ConsolePanel.h"
#include "../editor/panels/AssetPanel.h"
#include "../editor/panels/PerformancePanel.h"

#include "../graphics/geometry/ray.h"

namespace Lengine {
    

    class EditorLayer {
    public:
        EditorLayer(
            LogBuffer& buffer,
            SceneManager& sceneManager,
            Camera3d& camera,
            InputManager& inputManager,
            AssetManager& assetManager,
            Window& window,
            Renderer& renderer            
            );
        ~EditorLayer() = default;

        void OnAttach();
        void OnImGuiRender();  
        void OnDetach();

        // Access viewport panel (camera, input needs this)
        ViewportPanel& GetViewportPanel() { return viewportPanel; }
        PerformancePanel& GetPerformancePanel() { return performancePanel; }

        // Editor manipulation
        void checkForHoveredEntity(const glm::vec3& rayDir, const glm::vec3& rayOrigin);
        void selectHoveredEntity();
        void deselectAllEntities();
        void HandleDrag();
        void HandleMouseWheel(const int& mousewheelY);
        void HandleKeyboardShortcuts(const SDL_Keycode& key);

        
    private:
        void BeginDockspace();
        void SetupDefaultLayout();

        // Selection state
        Entity* selectedEntity = nullptr;
        Entity* hoveredEntity = nullptr;

        bool leftMousePressed = false;
        bool leftMouseDown = false;
        bool leftMouseReleased = false;

        // Dragging
        bool dragActive = false;
        glm::vec3 dragPlaneNormal = glm::vec3(0, 1, 0);
        float dragPlaneY = 0.0f;
        glm::vec3 dragStartPoint;
        glm::vec3 dragOffset;

        bool layoutInitialized = false;
    private:
        // Panels
        ViewportPanel viewportPanel;
        SceneHierarchyPanel hierarchyPanel;
        InspectorPanel inspectorPanel;
        ConsolePanel consolePanel;
        AssetPanel assetPanel;
        PerformancePanel performancePanel;

        // External engine systems (not owned)
        SceneManager& sceneManager;
        Camera3d& camera;
        InputManager& inputManager;
        AssetManager& assetManager;
        Window& window;
        Renderer& renderer;
    private:
        // Data sending to panels

        void sendDataToSceneHeirarchyPanel();
        void sendDataToInspectorPanel();

        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path assestPath = std::filesystem::absolute(exePath / "../assets");

    };

}


