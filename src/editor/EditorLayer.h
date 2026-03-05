#pragma once

#define IMGUI_ENABLE_DOCKING

#include "imgui.h"
#include "imgui_internal.h"

#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../platform/KeyBindings.h"
#include "../platform/Window.h"

#include "../editor/panels/ViewportPanel.h"
#include "../editor/panels/SceneHeirarchyPanel.h"
#include "../editor/panels/InspectorPanel.h"
#include "../editor/panels/ConsolePanel.h"
#include "../editor/panels/AssetPanel.h"
#include "../editor/panels/PerformancePanel.h"
#include "../editor/panels/RendererSettingsPanel.h"

#include "../graphics/geometry/ray.h"
#include "../graphics/geometry/Gizmos.h"

namespace Lengine {
   

    struct GizmoDragState {
        bool isDragging = false;
        GizmoAxis activeAxis = GizmoAxis::None;
        GizmoAxis hoveredAxis = GizmoAxis::None;

        glm::vec3 dragStartWorld;     // world-space hit point
        glm::vec3 entityStartPos;     // entity position at drag start
        glm::vec3 axisDir;            // locked axis direction (world)
    };
    class EditorLayer {
    public:
        EditorLayer(
            LogBuffer& buffer,
            SceneManager& sceneManager,
            GizmoRenderer& gizmoRndr,
            Camera3d& camera,
            InputManager& inputManager,
            AssetManager& assetManager,
            RenderSettings& rndrSett,
            const glm::i32vec2 resolution,
            RuntimeStats& stats
            );
        ~EditorLayer() = default;

        void OnAttach();
        void OnImGuiRender(const uint32_t& finalImage);
        void OnDetach();

        // Access viewport panel (camera, input needs this)
        ViewportPanel& GetViewportPanel() { return viewportPanel; }
        PerformancePanel& GetPerformancePanel() { return performancePanel; }

        // Editor manipulation
        void CheckForHoveredEntity();
        void BeginArrowDrag();
        GizmoAxis getHoveredGizmoAxis();

        void selectHoveredEntity();
        void deselectAllEntities();
        void HandleArrowDrag();
        void endArrowDrag();
        void HandleMouseWheel(const int& mousewheelY);
        void HandleKeyboardShortcuts(const SDL_Keycode& key);
        
        EditorConfig config;


    private:
        void BeginDockspace();
        void SetupDefaultLayout();

        void HandleAssetEditorClear();


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
        RendererSettingsPanel rendererSettingsPanel;

        // External engine systems (not owned)
        SceneManager& sceneManager;
        GizmoRenderer& gizmoRenderer;
        Camera3d& camera;
        InputManager& inputManager;
        AssetManager& assetManager;
        RenderSettings& renderSettings;
    private:
       
        std::filesystem::path exePath = std::filesystem::current_path();
        std::filesystem::path assestPath = Paths::Assets;
    private:
        // Keyboard 
        float movementSpeed = 0.01f;
        float movementSpeedMultiplier = 1.0f;

        void unselectAllEntites();
        bool isAnyEntitySelected();
    
    private:
        AxisCapsule capX;
        AxisCapsule capY;
        AxisCapsule capZ;

        GizmoDragState state;
        float closestT = FLT_MAX;
        GizmoAxis chosenAxis = GizmoAxis::None;
    private:
    };

}


