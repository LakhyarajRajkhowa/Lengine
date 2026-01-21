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

static bool openCreateScenePopup = false;
static char NewSceneName[128] = "NewScene";
static bool openRenameScenePopup = false;
static char RenameSceneBuffer[128];
static Scene* sceneToRename = nullptr;

void SceneHierarchyPanel::OnImGuiRender() {
    ImGui::Begin("Hierarchy");

    auto& allScenes = sceneManager.getScenes();
    Scene* activeScene = sceneManager.getActiveScene();

    ImGuiTreeNodeFlags rootFlags =
        ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanFullWidth;

    bool rootOpen = ImGui::TreeNodeEx("Scenes", rootFlags);

    // ---- RIGHT CLICK ON ROOT NODE ----
    if (ImGui::BeginPopupContextItem("ScenesRootPopup"))
    {
        if (ImGui::MenuItem("Create Scene"))
        {
            openCreateScenePopup = true;
        }
        ImGui::EndPopup();
    }

    // Attach popup to last item
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("ScenesRootPopup");
    }


    // Root node
    if (rootOpen)
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

                if (ImGui::MenuItem("Rename Scene"))
                {
                    sceneToRename = scene;
                    strncpy(RenameSceneBuffer,
                        scene->getName().c_str(),
                        IM_ARRAYSIZE(RenameSceneBuffer));
                    openRenameScenePopup = true;
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

                // Create Queued Entities
                while (!createdEntityQueue.empty()) {
                    scene->addEntity(
                        std::unique_ptr<Entity>(createdEntityQueue.front().first),
                        createdEntityQueue.front().second
                    );

                    createdEntityQueue.pop();
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
                        if (ImGui::BeginPopupContextItem(label.c_str()))
                        {
                            if (ImGui::MenuItem("Create Copy"))                       
                            {
                                Entity* clone = entity->Clone();
                                createdEntityQueue.push(std::pair(clone, entity->getID()));
                            }
                            ImGui::EndPopup();
                        }

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

    drawRenameScenePopup();
    drawCreateScenePopup();

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
                    selectedType,   // type
                    UUID()       // entityID
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

void SceneHierarchyPanel::drawCreateScenePopup()
{
    if (openCreateScenePopup)
    {
        ImGui::OpenPopup("Create New Scene");
        openCreateScenePopup = false;
    }

    if (ImGui::BeginPopupModal("Create New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Scene Name");
        ImGui::InputText("##SceneName", NewSceneName, IM_ARRAYSIZE(NewSceneName));
        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (strlen(NewSceneName) > 0)
            {
                Scene* newScene =
                    assetManager.createScene(NewSceneName, Paths::GameScenes);

                sceneManager.getScenes().insert(newScene);
                sceneManager.setActiveScene(newScene);

                strcpy(NewSceneName, "NewScene");
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchyPanel::drawRenameScenePopup()
{
    if (openRenameScenePopup)
    {
        ImGui::OpenPopup("Rename Scene");
        openRenameScenePopup = false;
    }

    if (ImGui::BeginPopupModal("Rename Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("New Scene Name");
        ImGui::InputText("##RenameScene", RenameSceneBuffer, IM_ARRAYSIZE(RenameSceneBuffer));
        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)))
        {
            if (sceneToRename && strlen(RenameSceneBuffer) > 0)
            {
               

                sceneToRename->rename(RenameSceneBuffer);

                ImGui::CloseCurrentPopup();
                sceneToRename = nullptr;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            sceneToRename = nullptr;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

