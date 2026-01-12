#include "EditorLayer.h"

namespace Lengine {

    EditorLayer::EditorLayer(
        LogBuffer& buffer,
        SceneManager& scnMgr,
        GizmoRenderer& gizmoRndr,
        Camera3d& cam,
        InputManager& inputMgr,
        AssetManager& assetMgr,
        Window& win,
        RenderSettings& rndrSett,
        glm::vec2 resolution
    )
        :camera(cam),
        sceneManager(scnMgr),
        gizmoRenderer(gizmoRndr),
        inputManager(inputMgr),
        assetManager(assetMgr),
        window(win),
        renderSettings(rndrSett),
        viewportPanel(cam, resolution),     
        hierarchyPanel(cam,  scnMgr,assetMgr, selectedEntity),
        inspectorPanel(scnMgr, assetMgr),
        consolePanel(buffer),
        assetPanel(Paths::ActiveGameFolder, assetMgr),
        rendererSettingsPanel(rndrSett)       
    {
    }

    void EditorLayer::OnAttach() {
        // nothing for now
    }

    void EditorLayer::OnDetach() {
        // cleanup if you want later
    }

    void EditorLayer::OnImGuiRender() {

        BeginDockspace();

        if (!layoutInitialized) {
            // SetupDefaultLayout(); 
            layoutInitialized = true;
        }

        //  Render panels
        if (!viewportPanel.viewportFullscreen) {
            viewportPanel.OnImGuiRender();
            hierarchyPanel.OnImGuiRender();
            inspectorPanel.OnImGuiRender();
            consolePanel.OnImGuiRender();
            assetPanel.OnImGuiRender();
            performancePanel.OnImGuiRender();
            rendererSettingsPanel.OnImGuiRender();
        }
        else {

            config.editingMode = true;
            viewportPanel.RenderFullscreen();
            performancePanel.OnImGuiRender();
        }

    }

    void EditorLayer::checkForHoveredEntity() {
        hoveredEntity = nullptr;

        ImVec2 mouse = viewportPanel.getMousePosInViewport();
        ImVec2 mouse2 = viewportPanel.getMousePosInImage();
        ImVec2 vpSize = viewportPanel.GetViewportSize();

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        glm::vec3 rayDir = ComputeRayDirection(
            mouse.x,
            mouse.y,
            vpSize.x,
            vpSize.y,
            view,
            projection
        );

        glm::vec3 rayOrigin = camera.getCameraPosition();
        const auto& entities = sceneManager.getActiveScene()->getEntities();
        float closest = 999999.0f;

        for (auto& e : entities) {

            glm::vec3 pos = e->getTransform().position;
            glm::vec3 scale = e->getTransform().scale;


            // --------------------------------------------------------
            // CASE 1: Entity has NO mesh assigned
            // --------------------------------------------------------
            if (!e->getMeshID())
            {
                float radius = 1.0f; // default sphere
                if (rayIntersectsSphere(rayOrigin, rayDir, pos, radius))
                {
                    float dist = glm::distance(rayOrigin, pos);
                    if (dist < closest)
                    {
                        closest = dist;
                        hoveredEntity = e.get();
                       
                    }
                }
            }


            
            else if (e->getMeshID())
            {
                Mesh* mesh = assetManager.getMesh(e->getMeshID());


                // --------------------------------------------------------
                // CASE 2: Entity HAS a mesh ID → try to get mesh
                // --------------------------------------------------------
                if (!mesh) {
                    float radius = 1.0f;
                    if (rayIntersectsSphere(rayOrigin, rayDir, pos, radius))
                    {
                        float dist = glm::distance(rayOrigin, pos);
                        if (dist < closest)
                        {
                            closest = dist;
                            hoveredEntity = e.get();
                        }
                    }
                }
                else {
                    // --------------------------------------------------------
                    // CASE 3: Mesh exists 
                    // --------------------------------------------------------
                    glm::vec3 localCenter = mesh->getLocalCenter();
                    float localRadius = mesh->getBoundingRadius();

                    // World-space conversion
                    glm::vec3 worldCenter = pos + localCenter * scale;

                    float maxScale = glm::max(scale.x, glm::max(scale.y, scale.z));
                    float worldRadius = localRadius * maxScale;

                    // Ray-sphere intersection
                    if (rayIntersectsSphere(rayOrigin, rayDir, worldCenter, worldRadius))
                    {
                        float dist = glm::distance(rayOrigin, worldCenter);
                        if (dist < closest)
                        {
                            closest = dist;

                            hoveredEntity = e.get();

                        }
                    }

                }
            }

          
        }
    }

    
    void EditorLayer::selectHoveredEntity() {
        checkForHoveredEntity();
        if (hoveredEntity == nullptr) {
            for (auto& entity : sceneManager.getActiveScene()->getEntities()) {
                entity->isSelected = false;
            }
            selectedEntity = nullptr;        
            return;
        }
        for (auto& other : sceneManager.getActiveScene()->getEntities()) {
            other->isSelected = false;
        }
        selectedEntity = hoveredEntity;
        selectedEntity->isSelected = true;       
    }
    GizmoAxis EditorLayer::getHoveredGizmoAxis()
    {
        ImVec2 mouse = viewportPanel.getMousePosInViewport();
        ImVec2 vpSize = viewportPanel.GetViewportSize();

        glm::vec3 rayDir = ComputeRayDirection(
            mouse.x, mouse.y,
            vpSize.x, vpSize.y,
            camera.getViewMatrix(),
            camera.getProjectionMatrix()
        );

        glm::vec3 rayOrigin = camera.getCameraPosition();

        float closestT = FLT_MAX;
        GizmoAxis result = GizmoAxis::None;

        auto& capX = gizmoRenderer.getCapsule(GizmoAxis::X);
        auto& capY = gizmoRenderer.getCapsule(GizmoAxis::Y);
        auto& capZ = gizmoRenderer.getCapsule(GizmoAxis::Z);

        float t;


        if (rayIntersectsCapsule(rayOrigin, rayDir, capX.start, capX.end, capX.radius, t) && t < closestT) {
            closestT = t;
            result = GizmoAxis::X;
            capX.hovered = true;
            capY.hovered = false;
            capZ.hovered = false;

        }

       if (rayIntersectsCapsule(rayOrigin, rayDir, capY.start, capY.end, capY.radius, t) && t < closestT) {
            closestT = t;
            result = GizmoAxis::Y;
            capY.hovered = true;
            capX.hovered = false;
            capZ.hovered = false;
        }

        if (rayIntersectsCapsule(rayOrigin, rayDir, capZ.start, capZ.end, capZ.radius, t) && t < closestT) {
            closestT = t;
            result = GizmoAxis::Z;
            capZ.hovered = true;
            capX.hovered = false;
            capY.hovered = false;
        }

        if (result == GizmoAxis::None) {
            capX.hovered = false;
            capY.hovered = false;
            capZ.hovered = false;
        }
        
        return result;
    }
    
    void EditorLayer::beginArrowDrag()
    {
        if (!selectedEntity) return;

       
        GizmoAxis hovered = getHoveredGizmoAxis();
        if (hovered == GizmoAxis::None)
            return;

        ImVec2 mouse = viewportPanel.getMousePosInViewport();
        ImVec2 vpSize = viewportPanel.GetViewportSize();

        glm::vec3 rayDir = ComputeRayDirection(
            mouse.x, mouse.y,
            vpSize.x, vpSize.y,
            camera.getViewMatrix(),
            camera.getProjectionMatrix()
        );

        glm::vec3 rayOrigin = camera.getCameraPosition();

        state.activeAxis = hovered;

        state.axisDir =
            hovered == GizmoAxis::X ? glm::vec3(1, 0, 0) :
            hovered == GizmoAxis::Y ? glm::vec3(0, 1, 0) :
            glm::vec3(0, 0, 1);

        state.axisDir = glm::normalize(state.axisDir);
        state.entityStartPos = selectedEntity->getTransform().position;

        state.dragStartWorld = computeAxisPlaneHit(
            rayOrigin,
            rayDir,
            state.axisDir,
            state.entityStartPos,
            camera.getCameraPosition()
        );

        state.isDragging = true;
    }

    void EditorLayer::deselectAllEntities() {
        selectedEntity = nullptr;
    }

    void EditorLayer::HandleArrowDrag()
    {
        if (!state.isDragging || !selectedEntity)
            return;

        ImVec2 mouse = viewportPanel.getMousePosInViewport();
        ImVec2 vpSize = viewportPanel.GetViewportSize();

        glm::vec3 rayDir = ComputeRayDirection(
            mouse.x, mouse.y,
            vpSize.x, vpSize.y,
            camera.getViewMatrix(),
            camera.getProjectionMatrix()
        );

        glm::vec3 rayOrigin = camera.getCameraPosition();

        glm::vec3 hit = computeAxisPlaneHit(
            rayOrigin,
            rayDir,
            state.axisDir,
            state.entityStartPos,
            rayOrigin
        );

        glm::vec3 delta = hit - state.dragStartWorld;
        float movement = glm::dot(delta, state.axisDir);

        selectedEntity->getTransform().position =
            state.entityStartPos + state.axisDir * movement;
    }
    void EditorLayer::endArrowDrag()
    {
        state.isDragging = false;
        state.activeAxis = GizmoAxis::None;
    }


    void EditorLayer::HandleMouseWheel(const int& mouseWheelY) {

        if (!selectedEntity) return;

        auto& scale = selectedEntity->getTransform().scale;

        if (mouseWheelY > 0) {
            // scale up
            scale += (scale.x < 0.1f) ?
                ((scale.x < 0.01f) ? glm::vec3(0.001f) : glm::vec3(0.01f)) :
                glm::vec3(0.1f);
        }
        else if (mouseWheelY < 0) {
            // scale down
            scale -= (scale.x < 0.1f) ?
                ((scale.x < 0.01f) ? glm::vec3(0.001f) : glm::vec3(0.01f)) :
                glm::vec3(0.1f);
        }
    }


    void EditorLayer::HandleKeyboardShortcuts(const SDL_Keycode& key)
    {
        if (!selectedEntity) return;

        auto& pos = selectedEntity->getTransform().position;

        float speed = movementSpeed *
            (inputManager.isKeyDown(EditorKeys::FastMove) ? 5.0f : 1.0f);

        glm::vec3 camForward = camera.getForward();
        camForward.y = 0.0f;
        camForward = glm::normalize(camForward);

        glm::vec3 camRight = camera.getRight();
        camRight.y = 0.0f;
        camRight = glm::normalize(camRight);

        if (inputManager.isKeyDown(key))
        {
            switch (key)
            {
            case EditorKeys::MovePosZ: 
                pos += camForward * speed;
                break;

            case EditorKeys::MoveNegZ:
                pos -= camForward * speed;
                break;

            case EditorKeys::MovePosX: 
                pos += camRight * speed;
                break;

            case EditorKeys::MoveNegX: 
                pos -= camRight * speed;
                break;
            case EditorKeys::MovePosY:
                pos.y += movementSpeedMultiplier * speed;
                break;

            case EditorKeys::MoveNegY: 
                pos.y -= movementSpeedMultiplier * speed;
                break;
            }
        }
        if (inputManager.isKeyPressed(key))
        {
            switch (key)
            {
            case EditorKeys::Delete:
                sceneManager.getActiveScene()->removeEntity(selectedEntity->getID());
                selectedEntity = nullptr;
                break;
            }
        }
    }


    // Dockspace
    void EditorLayer::BeginDockspace() {

        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::Begin("MainDockspace", nullptr, window_flags);

        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MainDockspaceID");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0));

        ImGui::End();
    }

    
    // Default Layout 
    void EditorLayer::SetupDefaultLayout() {

        ImGuiID dockspace_id = ImGui::GetID("MainDockspaceID");

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        // Split into 3 columns: Hierarchy | Viewport | Inspector
        ImGuiID dock_left, dock_center, dock_right;

        // Left 20%
        dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, nullptr, &dock_center);

        // Right 20%
        dock_right = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Right, 0.20f, nullptr, &dock_center);

        // Optional: Bottom Console (25% height)
        ImGuiID dock_bottom;
        dock_bottom = ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Down, 0.25f, nullptr, &dock_center);

        // Assign windows
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("Viewport", dock_center);
        ImGui::DockBuilderDockWindow("Console", dock_bottom);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    bool EditorLayer::isAnyEntitySelected() {
        auto& entities = sceneManager.getActiveScene()->getEntities();

        for (auto& e : entities) {
            if (e->isSelected)
                return true;
        }
        return false;
    }

    void EditorLayer::unselectAllEntites() {
        auto& entities = sceneManager.getActiveScene()->getEntities();

        for (auto& e : entities) {
            e->isSelected = false;
        }
    }

}
