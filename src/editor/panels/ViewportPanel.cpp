#include "ViewportPanel.h"
#include <iostream>

using namespace Lengine;

    
    void ViewportPanel::ClearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void ViewportPanel::OnImGuiRender(const uint32_t finalImage)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        ImGui::Separator();
         
        if (ImGui::Button(camera.isFixed ? "Fix Camera: ON" : "Fix Camera: OFF"))
            camera.isFixed = !camera.isFixed;

        ImGui::SameLine();
        if (ImGui::Button("Fullscreen Mode"))
            viewportFullscreen = true;
        ImGui::SameLine();

        static const char* viewportModeLabels[] = {
           "First",
           "Second",
           "Third"

        };

        ViewportMode currentMode = mode;
        int currentModeIndex = static_cast<int>(currentMode);
        ImGui::PushItemWidth(140.0f);   

        if (ImGui::Combo("Viewport Mode", &currentModeIndex,
            viewportModeLabels,
            static_cast<int>(ViewportMode::count)))
        {
            camera.controlMode = static_cast<CameraControlMode>(currentModeIndex);
            mode = static_cast<ViewportMode>(currentModeIndex);

        }
        ImGui::PopItemWidth();



        ImGui::Separator();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_ViewportSize = { avail.x, avail.y };
        m_ViewportPos = ImGui::GetWindowPos();

      

        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0)
        {
            if (m_ViewportSize.x != m_LastViewportSize.x ||
                m_ViewportSize.y != m_LastViewportSize.y)
            {
                
                m_LastViewportSize = m_ViewportSize;
                camera.setAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
            }
        }
        

        GLuint texID = finalImage;
        

         ImGui::Image(
             (void*)(intptr_t)texID,
             ImVec2(m_ViewportSize.x, m_ViewportSize.y),
             ImVec2(0, 1),
             ImVec2(1, 0)
         );

         m_Focused = ImGui::IsItemFocused();
         m_Hovered = ImGui::IsItemHovered();
         imagePos = ImGui::GetItemRectMin();
         ImVec2 mousePos = ImGui::GetMousePos();
  
         mouseInViewport = { mousePos.x - imagePos.x, mousePos.y - imagePos.y };
        

           // DrawTransformGizmo();


        ImGui::End();
        ImGui::PopStyleVar();

    }



    void ViewportPanel::RenderFullscreen(const uint32_t finalImage)
    {
        camera.isFixed = false;
        camera.setAspectRatio(fullscreenAspectRatio);

        ImGuiViewport* vp = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(vp->Size);
        ImGui::SetNextWindowViewport(vp->ID);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoDocking;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("ViewportFullscreen", nullptr, flags);

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            viewportFullscreen = false;
        
        GLuint texID =  finalImage;

        ImVec2 avail = ImGui::GetContentRegionAvail();

        float availAspect = avail.x / avail.y;

        ImVec2 imageSize;
        if (availAspect > fullscreenAspectRatio)
        {
            imageSize.y = avail.y;
            imageSize.x = avail.y * fullscreenAspectRatio;
        }
        else
        {
            imageSize.x = avail.x;
            imageSize.y = avail.x / fullscreenAspectRatio;
        }

        ImVec2 cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(
            cursor.x + (avail.x - imageSize.x) * 0.5f,
            cursor.y + (avail.y - imageSize.y) * 0.5f
        ));

        ImGui::Image(
            (void*)(intptr_t)texID,
            imageSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        ImGui::End();
        ImGui::PopStyleVar();
    }

   void ViewportPanel::DrawTransformGizmo()
{
    if (!EditorSelection::GetEntity() || !sceneManager.getActiveScene())
        return;

    UUID selectedEntity = EditorSelection::GetEntity();

    auto* scene = sceneManager.getActiveScene();

    if (!scene->Transforms().Has(selectedEntity))
        return;

    TransformComponent& transform = scene->Transforms().Get(selectedEntity);

    // Camera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    // --- Configure gizmo ---
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();

    ImGuizmo::SetRect(
        imagePos.x,
        imagePos.y,
        m_ViewportSize.x,
        m_ViewportSize.y
    );

    static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;

    if (ImGui::IsKeyPressed(ImGuiKey_T))
        operation = ImGuizmo::TRANSLATE;

    if (ImGui::IsKeyPressed(ImGuiKey_E))
        operation = ImGuizmo::ROTATE;

    if (ImGui::IsKeyPressed(ImGuiKey_R))
        operation = ImGuizmo::SCALE;

    // ---- Use world matrix ----
    glm::mat4 transformMatrix = transform.localMatrix;

    // ---- Draw gizmo ----
    ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(projection),
        operation,
        ImGuizmo::WORLD,
        glm::value_ptr(transformMatrix)
    );

  
    if (ImGuizmo::IsUsing())
    {
        glm::vec3 translation = glm::vec3(transformMatrix[3]);

        std::cout << "Translation: "
            << translation.x << " "
            << translation.y << " "
            << translation.z << "\n";


        // ---- APPLY ----
        transform.localPosition = translation;
        //transform.localScale = scale;

        //glm::mat3 rotMatrix = glm::mat3(transformMatrix);
        //transform.localRotation = glm::normalize(glm::quat_cast(rotMatrix));

        transform.localDirty = true;
        transform.worldDirty = true;
        TransformSystem::Dirty = true;
    }
}