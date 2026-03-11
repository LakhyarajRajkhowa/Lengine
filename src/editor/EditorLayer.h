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

#include "../editor/EditorManipulation.h"

#include "../graphics/geometry/ray.h"
#include "../graphics/geometry/Gizmos.h"

namespace Lengine {
   
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
            RuntimeStats& stats
            );
        ~EditorLayer() = default;

        void OnAttach();
        void OnImGuiRender(const uint32_t& finalImage);
        void OnDetach();

        ViewportPanel& GetViewportPanel() { return viewportPanel; }
        PerformancePanel& GetPerformancePanel() { return performancePanel; }
        
        EditorConfig config;
        EditorManipulator manipulator;


    private:
        // Selection state
        Entity* selectedEntity = nullptr;
        Entity* hoveredEntity = nullptr;

        bool layoutInitialized = false;

        void BeginDockspace();
        void SetupDefaultLayout();

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

    
    };

}


