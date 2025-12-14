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
        Mesh* mesh = assetManager.getMesh(entity->getMeshID());
        sceneManager.getActiveScene()->assignDefaultMaterials(entity, mesh);
        //assetManager.linkMaterialInstance(sceneManager.getActiveScene(),  entity);
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
            sceneManager.getActiveScene()->assignDefaultMaterials(entity, mesh);
            //assetManager.linkMaterialInstance(sceneManager.getActiveScene(),  entity);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndGroup();

    // Lightning

    ImGui::Separator();
    ImGui::Text("Lightning");
    ImGui::Separator();

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
    Mesh* mesh = assetManager.getMesh(entity->getMeshID());
    if (!mesh) return;

    for (int i = 0; i < mesh->subMeshes.size(); i++)
    {
        auto& sm = mesh->subMeshes[i];

        ImGui::PushID(i);  

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_FramePadding |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        bool open = ImGui::TreeNodeEx(
            "##SubMeshNode",  
            flags,
            "%s", sm.getName().empty() ? "SubMesh" : sm.getName().c_str()
        );

        // Hover detection
        if (ImGui::IsItemHovered()) 
            sm.isHovered = true;
        else 
            sm.isHovered = false;
        
        if (open)
        {
            sm.isSelected = true;

            // --- Material instance ---
            UUID instID = entity->getMaterialInstanceUUIDs().at(sm.getName());
            MaterialInstance& inst =
                sceneManager.getActiveScene()->getMaterialInstance(instID);
            const Material* baseMat =
                assetManager.getMaterial(inst.baseMaterial);

            // ---- Diffuse (Kd) ----
            glm::vec3 kd = inst.Kd.value_or(baseMat->Kd);
            if (ImGui::ColorEdit3("Diffuse (Kd)", glm::value_ptr(kd)))
                inst.Kd = kd; // override only if user edits

            if (inst.Kd.has_value() && ImGui::SmallButton("Reset Kd")) inst.Kd.reset();
            ImGui::Spacing();

            // ---- Ambient (Ka) ----
            glm::vec3 ka = inst.Ka.value_or(baseMat->Ka);
            if (ImGui::ColorEdit3("Ambient (Ka)", glm::value_ptr(ka)))
                inst.Ka = ka;
            if (inst.Ka.has_value() && ImGui::SmallButton("Reset Ka")) inst.Ka.reset();
            ImGui::Spacing();

            // ---- Specular (Ks) ----
            glm::vec3 ks = inst.Ks.value_or(baseMat->Ks);
            if (ImGui::ColorEdit3("Specular (Ks)", glm::value_ptr(ks)))
                inst.Ks = ks;
            if (inst.Ks.has_value() && ImGui::SmallButton("Reset Ks")) inst.Ks.reset();
            ImGui::Spacing();

            // ---- Emissive (Ke) ----
            glm::vec3 ke = inst.Ke.value_or(baseMat->Ke);
            if (ImGui::ColorEdit3("Emissive (Ke)", glm::value_ptr(ke)))
                inst.Ke = ke;
            if (inst.Ke.has_value() && ImGui::SmallButton("Reset Ke")) inst.Ke.reset();
            ImGui::Spacing();

            // ---- Shininess (Ns) ----
            float ns = inst.Ns.value_or(baseMat->Ns);
            if (ImGui::DragFloat("Shininess (Ns)", &ns, 0.1f, 0.0f, 256.0f))
                inst.Ns = ns;
            if (inst.Ns.has_value() && ImGui::SmallButton("Reset Ns")) inst.Ns.reset();

            ImGui::Spacing();
            ImGui::TreePop();
        }
        else {
            sm.isSelected = false;
            ImGui::PopID();
            continue;
        }
        ImGui::PopID();
    }    
    

   
}
