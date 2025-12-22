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
            static bool openModelPopup = false;
            // --- RIGHT CLICK MENU FOR SCENE ---
            if (ImGui::BeginPopupContextItem(scene->getName().c_str()))
            {
                if (ImGui::MenuItem("Set Active"))
                    sceneManager.setActiveScene(scene);

                
                if (scene == activeScene) {
                    if (ImGui::MenuItem("Save Scene")) {
                        assetManager.saveScene(*scene, Paths::GameScenes);
                        assetManager.saveSceneAssetRegistryForScene(*scene, Paths::GameAssetRegistryFolder);
                    }

                    if (ImGui::MenuItem("Add Entity")) {
                        ImGui::OpenPopup("Add New Entity");
                        openModelPopup = true;
                    }                       
                }
                ImGui::EndPopup();
            }

            // Modal for new entity creation
            if (openModelPopup) {
                ImGui::OpenPopup("Add New Entity");
                openModelPopup = false;
            }
            createNewModel();

            // --- ENTITIES UNDER THAT SCENE ---
            if (sceneOpen)
            {
                // Delete Queued Entitities
                while (!deletedEntityQueue.empty()) {
                    scene->removeEntity(deletedEntityQueue.front());
                    deletedEntityQueue.pop();
                }

                // ----- ENTITIES -----
                for (auto& entity : scene->getEntities())
                {
                    if (scene == activeScene)
                    {
                        ImGui::PushID(entity->getID());

                        // SELECTABLE ENTITY
                        bool isSelected = entity->isSelected;
                        std::string label = entity->getName() + "##" + std::to_string(entity->getID());
                       
                        bool entityVisible = entity->isVisible;
                        if (ImGui::Checkbox("##Visible", &entityVisible)) {
                            entity->isVisible = entityVisible;
                        }

                        ImGui::SameLine();
                        
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            // deselect all
                            for (auto& e : activeScene->getEntities())
                                e->isSelected = false;
                            entity->isSelected = true;
                        }

                        // Right-click menu for active scene entities
                        if (ImGui::BeginPopupContextItem(label.c_str()))
                        {
                            if (ImGui::MenuItem("Delete"))
                            {
                                deletedEntityQueue.push(entity->getID());
                                entity->isSelected = false;
                            }
                            ImGui::EndPopup();
                        }

                        ImGui::PopID();
                    }
                    else
                    {
                        // NON-INTERACTIVE ENTITY NAME
                        ImGui::Text("%s", entity->getName().c_str());
                    }
                }

                ImGui::TreePop();    
            }
            
        }

        ImGui::TreePop();
    }

   

    ImGui::End();
}




void SceneHierarchyPanel::createNewModel() {
    Scene* activeScene = sceneManager.getActiveScene();

    static char EntityName[128] = "MyEntity";
    static int entityTypeIndex = 0; // default selected type index
    const char* entityTypes[] = { "DefaultObject", "Light", "Camera" };

    if (ImGui::BeginPopupModal("Add New Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter Entity Name:");
        ImGui::InputText("##name", EntityName, IM_ARRAYSIZE(EntityName));
        ImGui::Separator();

        // Dropdown to select entity type
        ImGui::Text("Select Entity Type:");
        ImGui::Combo("##type", &entityTypeIndex, entityTypes, IM_ARRAYSIZE(entityTypes));

        ImGui::Separator();

        if (ImGui::Button("Create"))
        {
            if (strlen(EntityName) > 0) {
                // Convert index to EntityType enum
                EntityType selectedType = static_cast<EntityType>(entityTypeIndex);

                Entity* newEntity = activeScene->createEntity(
                    EntityName,
                    UUID(),       // meshID
                    UUID(),        // entityID
                    selectedType   // type
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

