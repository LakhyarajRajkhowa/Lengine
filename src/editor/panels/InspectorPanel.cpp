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
    ImGui::Text("Type");
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
        tr.rotation = glm::vec3(0.0f); 
    }

    glm::vec3 rotationDeg = glm::degrees(tr.rotation);

    if (ImGui::DragFloat3(
        "##Rotation",
        glm::value_ptr(rotationDeg),
        0.5f
    ))
    {
        tr.rotation = glm::radians(rotationDeg);
    }


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

    if (entity->getType() == EntityType::Light) {
        ImGui::Separator();
        ImGui::Text("Lighting");
        ImGui::Separator();

        ImGui::Spacing();

        // ---------------- LIGHT TYPE ----------------
        static const char* lightTypeLabels[] = {
            "Directional",
            "Point",
            "Spotlight"
            
        };

        LightType currentType = entity->getLight().type;
        int currentTypeIndex = static_cast<int>(currentType);

        if (ImGui::Combo("Light Type", &currentTypeIndex,
            lightTypeLabels,
            static_cast<int>(LightType::count)))
        {
            entity->getLight().setType(static_cast<LightType>(currentTypeIndex));
          
        }

        ImGui::Spacing();

        // ---------------- Position (Point only) ----------------
        if (currentType != LightType::Directional) {
            ImGui::Text("Position");
            ImGui::SameLine();
            ImGui::DragFloat3(
                "##LightPosition",
                glm::value_ptr(entity->getLight().position),
                0.05f
            );
        }

        // ---------------- Direction (Directional only) ----------------
        if (currentType == LightType::Directional || currentType == LightType::Spotlight) {
            ImGui::Text("Direction");
            ImGui::SameLine();
            ImGui::DragFloat3(
                "##LightDirection",
                glm::value_ptr(entity->getLight().direction),
                0.05f
            );
        }

        ImGui::Spacing();

        // ---------------- Colors ----------------
        ImGui::Text("Ambient");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightAmbient",
            glm::value_ptr(entity->getLight().ambient)
        );

        ImGui::Spacing();

        ImGui::Text("Diffuse");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightDiffuse",
            glm::value_ptr(entity->getLight().diffuse)
        );

        ImGui::Spacing();

        ImGui::Text("Specular");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightSpecular",
            glm::value_ptr(entity->getLight().specular)
        );

        ImGui::Spacing();

        // ---------------- Attenuation (Point only) ----------------
        if (currentType == LightType::Point || currentType == LightType::Spotlight) {
            ImGui::Separator();
            ImGui::Text("Attenuation");
            ImGui::Separator();

            ImGui::Text("Constant");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenConstant",
                &entity->getLight().constant,
                0.01f, 0.0f, 10.0f
            );

            ImGui::Text("Linear");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenLinear",
                &entity->getLight().linear,
                0.01f, 0.0f, 1.0f
            );

            ImGui::Text("Quadratic");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenQuadratic",
                &entity->getLight().quadratic,
                0.001f, 0.0f, 1.0f
            );

            if (currentType == LightType::Spotlight) {
                ImGui::Separator();
                ImGui::Text("Cutoff Angles");
                ImGui::Separator();

                ImGui::Text("Inner (degrees)");
                ImGui::SameLine();
                ImGui::DragFloat(
                    "##InnerCutoffAngle",
                    &entity->getLight().cutOffAngle,
                    0.1f, 0.0f, 360.0f
                );

                ImGui::Text("Outer (degrees)");
                ImGui::SameLine();
                ImGui::DragFloat(
                    "##OuterCutoffAngle",
                    &entity->getLight().outerCutOffAngle,
                    0.1f, 0.0f, 360.0f
                );
            }
        }

        return;
    }

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
                
                if (openSubmesh)
                {
                    sm.isSelected = true;

                    const uint32_t matIdx = sm.getMatIdx();
                    Scene* scene = sceneManager.getActiveScene();

                    // ---------- Fetch instance safely ----------
                    UUID instID = entity->getMaterialIndexInstIDs().at(matIdx);
                    MaterialInstance* inst = &scene->getMaterialInstance(instID);

                    // ---------- Validate base material ----------
                    const Material* baseMat = nullptr;

                    if (inst->baseMaterial != UUID::Null)
                        baseMat = assetManager.getMaterial(inst->baseMaterial);

                    // ---------- If base material missing → reset ----------
                    if (inst->baseMaterial != UUID::Null && !baseMat)
                    {
                        entity->getMaterialIndexUUIDs()[matIdx] = UUID::Null;
                        scene->assignDefaultMaterials(entity, mesh);

                        // re-fetch instance (old one is INVALID)
                        instID = entity->getMaterialIndexInstIDs().at(matIdx);
                        inst = &scene->getMaterialInstance(instID);

                        if (inst->baseMaterial != UUID::Null)
                            baseMat = assetManager.getMaterial(inst->baseMaterial);
                    }

                    // ---------- Base Material UI ----------
                    std::string baseMatName = baseMat ? baseMat->getName() : "None";

                    ImGui::Text("Base Material");
                    ImGui::Button((baseMatName + "##baseMaterial").c_str(),
                        { ImGui::GetContentRegionAvail().x, 28 });

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload =
                            ImGui::AcceptDragDropPayload("MATERIAL_ASSET"))
                        {
                            const TextureDragPayload* data =
                                static_cast<const TextureDragPayload*>(payload->Data);

                            if (!assetManager.getMaterial(data->id))
                                assetManager.loadMaterial(data->id, data->path);

                            inst->baseMaterial = data->id;
                            entity->getMaterialIndexUUIDs()[matIdx] = data->id;
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::Spacing();

                    // ---------- Helper lambda for texture slots ----------
                    auto DrawTextureSlot = [&](const char* label,
                        std::optional<UUID>& instSlot,
                        UUID baseSlot)
                        {
                            std::string texName = "None";

                            if (instSlot.has_value())
                            {
                                if (GLTexture* t = assetManager.getTexture(instSlot.value()))
                                    texName = t->name;
                            }
                            else if (baseSlot != UUID::Null)
                            {
                                if (GLTexture* t = assetManager.getTexture(baseSlot))
                                    texName = t->name;
                            }

                            ImGui::Text(label);
                            ImGui::Button((texName + std::string("##") + label).c_str(),
                                { ImGui::GetContentRegionAvail().x, 28 });

                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload =
                                    ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                                {
                                    const TextureDragPayload* data =
                                        static_cast<const TextureDragPayload*>(payload->Data);

                                    if (!assetManager.getTexture(data->id))
                                        assetManager.loadTexture(data->id, data->path);

                                    instSlot = data->id;
                                }
                                ImGui::EndDragDropTarget();
                            }

                            ImGui::Spacing();
                        };

                    // ---------- Texture Maps ----------
                    if (baseMat)
                    {
                        DrawTextureSlot("Diffuse Map", inst->map_kd, baseMat->map_Kd);
                        DrawTextureSlot("Specular Map", inst->map_ks, baseMat->map_Ks);
                        DrawTextureSlot("Normal Map", inst->map_bump, baseMat->map_bump);
                    }

                    // ---------- Color + Scalar helpers ----------
                    auto DrawVec3 = [&](const char* label,
                        std::optional<glm::vec3>& instVal,
                        const glm::vec3& baseVal)
                        {
                            glm::vec3 v = instVal.value_or(baseVal);
                            if (ImGui::ColorEdit3(label, glm::value_ptr(v)))
                                instVal = v;

                            if (instVal.has_value() &&
                                ImGui::SmallButton(("Reset##" + std::string(label)).c_str()))
                                instVal.reset();

                            ImGui::Spacing();
                        };

                    auto DrawFloat = [&](const char* label,
                        std::optional<float>& instVal,
                        float baseVal,
                        float min,
                        float max)
                        {
                            float v = instVal.value_or(baseVal);
                            if (ImGui::DragFloat(label, &v, 0.1f, min, max))
                                instVal = v;

                            if (instVal.has_value() &&
                                ImGui::SmallButton(("Reset##" + std::string(label)).c_str()))
                                instVal.reset();

                            ImGui::Spacing();
                        };

                    // ---------- Material Params ----------
                    if (baseMat)
                    {
                        DrawVec3("Diffuse (Kd)", inst->Kd, baseMat->Kd);
                        DrawVec3("Ambient (Ka)", inst->Ka, baseMat->Ka);
                        DrawVec3("Specular (Ks)", inst->Ks, baseMat->Ks);
                        DrawVec3("Emissive (Ke)", inst->Ke, baseMat->Ke);

                        DrawFloat("Normal Strength",
                            inst->normalStrength,
                            baseMat->normalStrength,
                            -10.0f, 10.0f);

                        DrawFloat("Shininess (Ns)",
                            inst->Ns,
                            baseMat->Ns,
                            0.0f, 256.0f);
                    }

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
