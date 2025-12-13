#include "InspectorPanel.h"
#include "../graphics/renderer/Renderer.h"
using namespace Lengine;

InspectorPanel::InspectorPanel(
    SceneManager& scnMgr,
    AssetManager& asstMgr,
    Renderer& rndr
) :
    sceneManager(scnMgr),
    assetManager(asstMgr),
    renderer(rndr)
{
}

void InspectorPanel::OnImGuiRender() {
    ImGui::Begin("Inspector");

    ImGui::Separator();

    for (auto& entity : sceneManager.getActiveScene()->getEntities()) {
        if (entity->isSelected) {
            selectedEntity = entity.get();
            break;
        }
        selectedEntity = nullptr;
    }
    
    DrawEntityInspector(selectedEntity, assetManager);

    ImGui::End();
}

void InspectorPanel::DrawEntityInspector(Entity* entity, AssetManager& assets)
{
    if (!entity) {
        ImGui::Text("No entity selected.");
        return;
    }
    
    // ---------------- NAME ----------------
    char buffer[256] = {};
    strcpy(buffer, entity->getName().c_str());

    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
        if (!buffer[0] == '\0') {
            entity->setName(buffer);
        }     
    }
    ImGui::Spacing();

    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::Separator();
    ImGui::Spacing();

    Transform& tr = entity->getTransform();   

    // ---------------- ENTITY TYPE ----------------
    static const char* entityTypeLabels[] = {
        "DefaultObject",
        "Light",
        "Camera"
    };

    EntityType currentType = entity->getType();
    int currentTypeIndex = static_cast<int>(currentType);

    if (ImGui::Combo("Entity Type", &currentTypeIndex,
        entityTypeLabels,
        static_cast<int>(EntityType::COUNT)))
    {
        entity->setType(static_cast<EntityType>(currentTypeIndex));
        assetManager.linkMaterials(entity);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::Separator();
    ImGui::Spacing();

    // ---------- POSITION ----------
    ImGui::Text("Position");
    ImGui::SameLine();

    if (ImGui::Button("Reset##pos")) {
        tr.position = { 0.0f, 0.0f, 0.0f };
    }
    ImGui::DragFloat3("##Position", glm::value_ptr(tr.position), 0.1f);


    // ---------- ROTATION ----------
    ImGui::Text("Rotation");
    ImGui::SameLine();

    if (ImGui::Button("Reset##rot")) {
        tr.rotation = { 0.0f, 0.0f, 0.0f };
    }
    ImGui::DragFloat3("##Rotation", glm::value_ptr(tr.rotation), 0.5f);


    // ---------- SCALE ----------
    ImGui::Text("Scale");
    ImGui::SameLine();

    if (ImGui::Button("Reset##scale")) {
        tr.scale = { 1.0f, 1.0f, 1.0f };
    }

    // Uniform scale toggle
    static bool uniformScale = true;

    ImGui::Checkbox("Uniform", &uniformScale);

    if (uniformScale)
    {
        float s = tr.scale.x;
        if (ImGui::DragFloat("##UniformScale", &s, 0.05f))
            tr.scale = { s, s, s };
    }
    else
    {
        ImGui::DragFloat3("##Scale", glm::value_ptr(tr.scale), 0.05f);
    }


    

    // ---------------- MESH BLOCK ----------------
    ImGui::Separator();
    ImGui::Text("Mesh");
    ImGui::Separator();
    ImGui::Spacing();
    // --- Draw a centered box ---
    ImGui::BeginGroup();

   
    // Mesh name string
    std::string meshName = "None";
    UUID meshID = entity->getMeshID();

    if (!meshID.isNull()) {
        Mesh* mesh = assets.getMesh(meshID);
        meshName = mesh ? mesh->name : "Invalid Mesh";
    }
    ImVec2 size = { ImGui::GetContentRegionAvail().x, 30 };
    ImGui::Button(meshName.c_str(), size);

    // --- Drag Drop Target ---
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_ASSET"))
        {
            const MeshDragPayload* data = (const MeshDragPayload*)payload->Data;

            UUID droppedID = data->id;
            std::string meshPath = data->path;

            Mesh* mesh = assetManager.getMesh(droppedID);
           
            if (!mesh) {
                assetManager.loadMesh(droppedID, meshPath);
                mesh = assetManager.getMesh(droppedID);
            }

            entity->setMeshID(droppedID);
            assetManager.linkMaterials(entity);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndGroup();

    // Lightning

    ImGui::Separator();
    ImGui::Text("Lightning");
    ImGui::Separator();

    ImGui::Spacing();

    ImGui::Text("Ambient Strength");
    ImGui::SameLine();
    ImGui::DragFloat("##AmbientStrength", &renderer.ambientStrength, 0.01f, 0.0f, 1.0f);

    ImGui::Spacing();

    ImGui::Text("Specular Strength");
    ImGui::SameLine();
    ImGui::DragFloat("##SpecularStrength", &renderer.specularStrength, 0.05f, 0.0f, 5.0f);

    ImGui::Spacing(); 

    ImGui::Text("Light Color");
    ImGui::SameLine();
    ImGui::ColorEdit3("Light Color", glm::value_ptr(renderer.lightColor));

    ImGui::Spacing();

    if (ImGui::Button(renderer.changeColor ? "Change Color: ON" : "Change Color: OFF")) {
        renderer.changeColor = !renderer.changeColor;
        if (!renderer.changeColor) {
            renderer.lightColor = glm::vec3(1.0f);
        }
    }
        


    // MATERIALS
    
    
    ImGui::Separator();
    ImGui::Text("Material");
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Text("Albedo");
    ImGui::Spacing();
    if (assetManager.getMesh(entity->getMeshID()))
        for (auto& sm : assetManager.getMesh(entity->getMeshID())->subMeshes) {
        
            ImGui::Text(sm.getName().c_str());
            ImGui::SameLine();
            //ImGui::ColorEdit3("Light Color: ", glm::value_ptr(sm.getMaterial()->objectColor));
            ImGui::Spacing();

        }

    
}
