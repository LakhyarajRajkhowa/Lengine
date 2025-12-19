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
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndGroup();

    // Lightning

    ImGui::Separator();
    ImGui::Text("Lightning");
    ImGui::Separator();

    ImGui::Spacing();

    ImGui::Text("Position");
    ImGui::SameLine();
    ImGui::DragFloat3("##LightPosition", glm::value_ptr(renderer.light.position), 0.05f);

    ImGui::Spacing();

    ImGui::Text("Direction");
    ImGui::SameLine();
    ImGui::DragFloat3("##LightDirection", glm::value_ptr(renderer.light.direction), 0.05f);

    ImGui::Spacing();

    ImGui::Text("Ambient");
    ImGui::SameLine();
    ImGui::ColorEdit3("Ambient", glm::value_ptr(renderer.light.ambient));

    ImGui::Spacing();

    ImGui::Text("Diffuse");
    ImGui::SameLine();
    ImGui::ColorEdit3("Diffuse", glm::value_ptr(renderer.light.diffuse));

    ImGui::Spacing();

    ImGui::Text("Specular");
    ImGui::SameLine();
    ImGui::ColorEdit3("Specular", glm::value_ptr(renderer.light.specular));

     // MATERIALS
       
    ImGui::Separator();
    ImGui::Text("Material");
    ImGui::Separator();
    ImGui::Spacing();
    Mesh* mesh = assetManager.getMesh(entity->getMeshID());
    if (!mesh) return;

    for (int i = 0; i < mesh->materialGroups.size(); i++)
    {

        auto& materialGroup = mesh->materialGroups[i];
        if (materialGroup.empty())
            continue;
        ImGui::PushID(i);  

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_FramePadding |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        unsigned int firstSmIdx = materialGroup.front();
        SubMesh& firstSm = mesh->subMeshes[firstSmIdx];

        bool open = ImGui::TreeNodeEx(
            "MaterialNode",
            flags,
            "%s", firstSm.getName().empty() ? "Material" : firstSm.getName().c_str()
        );

        // Hover detection
        if (ImGui::IsItemHovered()) {
            for (auto& smIdx : materialGroup) {
                entity->hoveredSubMeshes.insert(smIdx);
              //  mesh->subMeshes[smIdx].isHovered = true;
            }
        }
        else {
            for (auto& smIdx : materialGroup) {
                entity->hoveredSubMeshes.erase(smIdx);
               // mesh->subMeshes[smIdx].isHovered = false;
            }
        }
        
        if (open) {
            
            for (unsigned int i = 0; i < materialGroup.size(); i++) {
                unsigned int smIdx = materialGroup[i];
                SubMesh& sm = mesh->subMeshes[smIdx];

                ImGui::PushID(smIdx);

                bool submeshVisible = sm.isVisible;
                if (ImGui::Checkbox("##Visible", &submeshVisible)) {
                    sm.isVisible = submeshVisible;
                }

                ImGui::SameLine();

                bool openSubmesh = ImGui::TreeNodeEx(
                    "SubMeshNode",
                    flags,
                    "%s", sm.getName().empty() ? "SubMesh" : sm.getName().c_str()
                );
                if (ImGui::IsItemHovered()) {
                    entity->hoveredSubMeshes.insert(smIdx);
                }
                else {
                    entity->hoveredSubMeshes.erase(smIdx);
                }
                if (ImGui::IsItemHovered()) {
                    sm.isHovered = true;   
                }
                else {
                    sm.isHovered = false;                   
                }
                
                if (openSubmesh) {
                    sm.isSelected = true;

                    UUID instID = entity->getMaterialIndexUUIDs().at(sm.getMatIdx());
                    MaterialInstance& inst =
                        sceneManager.getActiveScene()->getMaterialInstance(instID);
                    const Material* baseMat =
                        assetManager.getMaterial(inst.baseMaterial);
                    
                    // Base Material
                    std::string baseMatName = "None";

                    if (inst.baseMaterial) {
                        baseMatName = assetManager.getMaterial(inst.baseMaterial)->getName();
                    }
                    ImVec2 btnSize = { ImGui::GetContentRegionAvail().x, 28 };
                    ImGui::Text("Base Material");
                    ImGui::Button((baseMatName + "##baseMaterial").c_str(), btnSize);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload =
                            ImGui::AcceptDragDropPayload("MATERIAL_ASSET"))
                        {
                            const TextureDragPayload* data =
                                (const TextureDragPayload*)payload->Data;

                            if (!assetManager.getMaterial(data->id)) {
                                if (assetManager.loadMaterial(data->id, data->path)) {
                                    inst.baseMaterial = data->id;
                                }
                            }
                            else {
                                inst.baseMaterial = data->id;
                            }
                           
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // Diffuse Map
                    {
                        std::string diffuseTexName = "None";

                        if (inst.map_kd.has_value()) {
                            GLTexture* instTex = assetManager.getTexture(inst.map_kd.value());
                            diffuseTexName = instTex->name;
                        }
                        else if (baseMat->map_Kd != UUID::Null) {
                            GLTexture* baseTex = assetManager.getTexture(baseMat->map_Kd);
                            diffuseTexName = baseTex->name;
                        }

                        ImVec2 btnSize = { ImGui::GetContentRegionAvail().x, 28 };
                        ImGui::Text("Diffuse Map");
                        ImGui::Button((diffuseTexName + "##diffuse").c_str(), btnSize);

                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload =
                                ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                            {
                                const TextureDragPayload* data =
                                    (const TextureDragPayload*)payload->Data;

                                UUID texID = data->id;
                                std::string texPath = data->path;
                                if (!assetManager.getTexture(texID)) {
                                    assetManager.loadTexture(texID, texPath);
                                }
                                inst.map_kd = texID;

                            }
                            ImGui::EndDragDropTarget();
                        }

                        

                        ImGui::Spacing();
                    }


                    // ---- Diffuse (Kd) ----
                    glm::vec3 kd = inst.Kd.value_or(baseMat->Kd);
                    if (ImGui::ColorEdit3("Diffuse (Kd)", glm::value_ptr(kd)))
                        inst.Kd = kd; 

                    if (inst.Kd.has_value() && ImGui::SmallButton("Reset Kd")) inst.Kd.reset();
                    ImGui::Spacing();

                    // ---- Ambient (Ka) ----
                    glm::vec3 ka = inst.Ka.value_or(baseMat->Ka);
                    if (ImGui::ColorEdit3("Ambient (Ka)", glm::value_ptr(ka)))
                        inst.Ka = ka;
                    if (inst.Ka.has_value() && ImGui::SmallButton("Reset Ka")) inst.Ka.reset();
                    ImGui::Spacing();

                    // Specular Map
                    {
                        std::string specularTexName = "None";

                        if (inst.map_ks.has_value()) {
                            GLTexture* instTex = assetManager.getTexture(inst.map_ks.value());
                            specularTexName = instTex->name;
                        }
                        else if (baseMat->map_Ks != UUID::Null) {
                            GLTexture* baseTex = assetManager.getTexture(baseMat->map_Ks);
                            specularTexName = baseTex->name;
                        }

                        ImVec2 btnSize = { ImGui::GetContentRegionAvail().x, 28 };
                        ImGui::Text("Specular Map");
                        ImGui::Button((specularTexName + "##specular").c_str(), btnSize);

                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload =
                                ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                            {
                                const TextureDragPayload* data =
                                    (const TextureDragPayload*)payload->Data;

                                UUID texID = data->id;
                                std::string texPath = data->path;
                                if (!assetManager.getTexture(texID)) {
                                    assetManager.loadTexture(texID, texPath);
                                }
                                inst.map_ks = texID;

                            }
                            ImGui::EndDragDropTarget();
                        }

                       

                        ImGui::Spacing();
                    }

                    // ---- Specular (Ks) ----
                    glm::vec3 ks = inst.Ks.value_or(baseMat->Ks);
                    if (ImGui::ColorEdit3("Specular (Ks)", glm::value_ptr(ks)))
                        inst.Ks = ks;
                    if (inst.Ks.has_value() && ImGui::SmallButton("Reset Ks")) inst.Ks.reset();
                    ImGui::Spacing();

                    // Normal Map
                    {
                        std::string normalTexName = "None";

                        if (inst.map_bump.has_value()) {
                            GLTexture* instTex = assetManager.getTexture(inst.map_bump.value());
                            normalTexName = instTex->name;
                        }
                        else if (baseMat->map_bump != UUID::Null) {
                            GLTexture* baseTex = assetManager.getTexture(baseMat->map_bump);
                            normalTexName = baseTex->name;
                        }

                        ImVec2 btnSize = { ImGui::GetContentRegionAvail().x, 28 };
                        ImGui::Text("Normal Map");
                        ImGui::Button((normalTexName + "##normal").c_str(), btnSize);

                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload =
                                ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                            {
                                const TextureDragPayload* data =
                                    (const TextureDragPayload*)payload->Data;

                                UUID texID = data->id;
                                std::string texPath = data->path;
                                if (!assetManager.getTexture(texID)) {
                                    assetManager.loadTexture(texID, texPath);
                                }
                                inst.map_bump = texID;

                            }
                            ImGui::EndDragDropTarget();
                        }


                        ImGui::Spacing();
                    }

                    // Normal strength
                    float normalStrength = inst.normalStrength.value_or(baseMat->normalStrength);
                    if (ImGui::DragFloat("Normal Strength", &normalStrength, 0.1f, -10.0f, 10.0f))
                        inst.normalStrength = normalStrength;
                    if (inst.normalStrength.has_value() && ImGui::SmallButton("Reset Ns")) inst.normalStrength.reset();
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
                
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        else {
            for (auto& smIdx : materialGroup) {
                mesh->subMeshes[smIdx].isSelected = false;

            }           
            ImGui::PopID();
            continue;
        }
        ImGui::PopID();
}    
    
    
}
