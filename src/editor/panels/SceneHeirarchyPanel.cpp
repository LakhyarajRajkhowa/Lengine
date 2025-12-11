#include "SceneHeirarchyPanel.h"
using namespace Lengine;
SceneHierarchyPanel::SceneHierarchyPanel(
    Camera3d& cam,
    SceneManager& scnMgr,
    AssetManager& assetMgr,
    Entity* selectedEntity
)
    :
    camera(cam),
    sceneManager(scnMgr),
    assetManager(assetMgr),
    m_SelectedEntity(selectedEntity)
{
}

void SceneHierarchyPanel::OnImGuiRender() {
    ImGui::Begin("Hierarchy");

    auto& allScenes = sceneManager.getScenes();
    Scene* activeScene = sceneManager.getActiveScene();

    // Root node
    if (ImGui::TreeNodeEx("Scenes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (Scene* scene : allScenes)
        {
            // Highlight active scene
            ImGuiTreeNodeFlags flags =
                (scene == activeScene ? ImGuiTreeNodeFlags_DefaultOpen : 0);
            bool sceneOpen = ImGui::TreeNodeEx(
                scene->getName().c_str(),
                flags | ImGuiTreeNodeFlags_OpenOnArrow
            );

            // --- RIGHT CLICK MENU FOR SCENE ---
            if (ImGui::BeginPopupContextItem(scene->getName().c_str()))
            {
                if (ImGui::MenuItem("Set Active"))
                    sceneManager.setActiveScene(scene);

                if (ImGui::MenuItem("Save Scene"))
                    assetManager.saveScene(*scene, "../TestGameFolder/scenes");

                if (ImGui::MenuItem("Add Entity") && scene == activeScene)
                    ImGui::OpenPopup("Add New Entity");

                ImGui::EndPopup();
            }

            // --- ENTITIES UNDER THAT SCENE ---
            if (sceneOpen && scene == activeScene)
            {
                for (auto& entity : scene->getEntities())
                {
                    bool isSelected = entity->isSelected;

                    if (ImGui::Selectable(entity->getName().c_str(), isSelected))
                    {
                        // deselect all entities in activeScene
                        for (auto& e : activeScene->getEntities())
                            e->isSelected = false;

                        entity->isSelected = true;
                    }

                    if (ImGui::BeginPopupContextItem(entity->getName().c_str()))
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            deletedEntityQueue.push(entity->getName());
                            entity->isSelected = false;
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    // Modal for new entity creation
    createNewModel();

    ImGui::End();
}




void SceneHierarchyPanel::createNewModel() {
    static char EntityName[128] = "MyEntity";

    if (ImGui::BeginPopupModal("Add New Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter Entity Name:");
        ImGui::InputText("##name", EntityName, IM_ARRAYSIZE(EntityName));
        ImGui::Separator();

        if (ImGui::Button("Create"))
        {
            if (strlen(EntityName) > 0) {

                activeScene->createEntity(
                    EntityName,
                    UUID(0)                   
                );

                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
