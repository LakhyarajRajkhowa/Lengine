#include "InspectorPanel.h"

using namespace Lengine;

InspectorPanel::InspectorPanel(
    SceneManager& scnMgr,
    AssetManager& asstMgr
) :
    sceneManager(scnMgr),
    assetManager(asstMgr)
{
}

void InspectorPanel::OnImGuiRender() {
    ImGui::Begin("Inspector");

    ImGui::Separator();

    selectedEntity = nullptr;
    for (auto& entity : sceneManager.getActiveScene()->getEntities()) {
        if (entity->isSelected) {
            selectedEntity = entity.get();
            break;
        }
    }
    
    DrawEntityInspector(selectedEntity, assetManager);

    ImGui::End();
}

// TODO : break down this func into parts
void InspectorPanel::DrawEntityInspector(Entity* entity, AssetManager& assets)
{
    Scene* scene = sceneManager.getActiveScene();
    if (!entity) {
        ImGui::Text("No entity selected.");
        return;
    }
    
    // ---------------- NAME ----------------
    char buffer[256] = {};
    strcpy(buffer, entity->getName().c_str());

    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
        if (buffer[0] != '\0') {
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

        Mesh* mesh = assetManager.getMesh(scene->MeshFilters().Get(entity->getID()).meshID);

        // have to do reassign materials based on new entity type
        scene->assignDefaultPBRMaterials(entity->getID(), mesh);
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
    //         ( in Degrees )
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
    bool& uniformScale = inspectorState.uniformScale;

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

    MeshRenderer& mr = scene->MeshRenderers().Get(entity->getID());
    MeshFilter& mf = scene->MeshFilters().Get(entity->getID());


    ImGui::Separator();
    ImGui::Text("Mesh");
    ImGui::Separator();
    ImGui::Spacing();
    // --- Draw a centered box ---
    ImGui::BeginGroup();

   
    // Mesh name string
    std::string meshName = "None";
    UUID meshID = mf.meshID;

    if (!meshID.isNull() && !mf.HasPendingMesh()) {
        Mesh* mesh = assets.getMesh(meshID);
        meshName = mesh ? mesh->name : "Invalid Mesh";
    }
    ImVec2 size = { ImGui::GetContentRegionAvail().x, 30 };
    ImGui::Button((meshName + "##Mesh").c_str(), size);

    // --- Drag Drop Target ---
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_ASSET"))
        {
            const MeshDragPayload* data = (const MeshDragPayload*)payload->Data;

            UUID droppedID = data->id;
            std::string meshPath = data->path;

            Mesh* mesh = assetManager.getMesh(droppedID);
           
            // if dropped mesh ID isnt loaded yet, then request for a load
            if (!mesh && assetManager.assetStates[droppedID] != AssetState::Loading) {
                assetManager.requestMeshLoad(droppedID, meshPath, mf);
                mf.RequestMesh(droppedID);
            }
            else {
                mf.meshID = droppedID;
                scene->assignDefaultPBRMaterials(entity->getID(), mesh);

            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndGroup();


 


    // Lightning

    if (entity->getType() == EntityType::Light) {

        auto& light = entity->getLight();
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

        LightType currentType = light.type;
        int currentTypeIndex = static_cast<int>(currentType);

        if (ImGui::Combo("Light Type", &currentTypeIndex,
            lightTypeLabels,
            static_cast<int>(LightType::count)))
        {
            light.setType(static_cast<LightType>(currentTypeIndex));
          
        }

        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Shadows");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button(light.castShadow ? "Cast Shadow ON" : "Cast Shadow OFF"))
        {
            light.castShadow = !light.castShadow;
        }


        ImGui::Spacing();
        ImGui::Separator();

        // ---------------- Colors ----------------

        ImGui::Text("Intensity");
        ImGui::SameLine();
        ImGui::DragFloat(
            "##Intensity",
            &light.intensity,
            0.1f, 0.0f, 100.0f
        );

        ImGui::Spacing();

        ImGui::Text("Diffuse");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightDiffuse",
            glm::value_ptr(light.diffuse)
        );

        ImGui::Spacing();

        ImGui::Text("Specular");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightSpecular",
            glm::value_ptr(light.specular)
        );

        ImGui::Spacing();

        // ---------------- Attenuation  ----------------

        if (currentType == LightType::Point || currentType == LightType::Spotlight) {
            ImGui::Separator();
            ImGui::Text("Attenuation");
            ImGui::Separator();

            ImGui::Text("Constant");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenConstant",
                &light.constant,
                0.01f, 0.0f, 10.0f
            );

            ImGui::Text("Linear");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenLinear",
                &light.linear,
                0.01f, 0.0f, 1.0f
            );

            ImGui::Text("Quadratic");
            ImGui::SameLine();
            ImGui::DragFloat(
                "##AttenQuadratic",
                &light.quadratic,
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
                    &light.cutOffAngle,
                    0.1f, 0.0f, 360.0f
                );

                ImGui::Text("Outer (degrees)");
                ImGui::SameLine();
                ImGui::DragFloat(
                    "##OuterCutoffAngle",
                    &light.outerCutOffAngle,
                    0.1f, 0.0f, 360.0f
                );
            }
        }

    }
    else {
        ImGui::Separator();
        ImGui::Text("Global Lightning");
        ImGui::Separator();

        ImGui::Spacing();

        // Global Ambient 
        ImGui::Text("Ambient");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##GlobalLightAmbient",
            glm::value_ptr(scene->getAmbientLighting())
        );
    }

     // MATERIALS

    ImGui::Separator();
    ImGui::Text("Material");
    ImGui::Separator();
    ImGui::Spacing();
    Mesh* mesh = assetManager.getMesh(mf.meshID);
    if (!mesh || mf.HasPendingMesh()) return;

    // Submesh group based on materials
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

      
        
        if (open) {
            // Display all submesh in a material group
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

              
                
                
                if (openSubmesh)
                {
                    

                    const uint32_t matIdx = sm.getMatIdx(); // matIdx could be retrived from the outer loop?        
                    UUID instID = mr.materialIndexToInstID.at(matIdx);
                    PBRMaterialInstance& inst = scene->getPbrMaterialInstance(instID);
                    const PBRMaterial* baseMat = nullptr;

                    // if baseMat id exist then check if we can get it 
                    if (inst.baseMaterial != UUID::Null)
                        baseMat = assetManager.getPBRMaterial(inst.baseMaterial);

                    // if baseMat id exist but we can't get it
                    // then reassign the material 
                    if (inst.baseMaterial != UUID::Null && !baseMat)
                    {
                        // assign null to the matID 
                        // becoz there is a check for null id in the assignDefaultMaterials()
                        // where if null id found the submesh just takes the material of the parent entity
                        mr.materialIndexToUUID[matIdx] = UUID::Null;
                        scene->assignDefaultPBRMaterials(entity->getID(), mesh);

                        instID = mr.materialIndexToInstID.at(matIdx);
                        inst = scene->getPbrMaterialInstance(instID);

                        // so here the new baseMat is just the material of the parent entity
                        if (inst.baseMaterial != UUID::Null)
                            baseMat = assetManager.getPBRMaterial(inst.baseMaterial);
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

                            // if material isnt loaded then load it
                            if (!assetManager.getPBRMaterial(data->id))
                                assetManager.loadPBRMaterial(data->id, data->path);

                            inst.baseMaterial = data->id;
                            mr.materialIndexToUUID[matIdx] = data->id;

                            
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::Spacing();
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
                        float vel,
                        float min,
                        float max)
                        {
                            float v = instVal.value_or(baseVal);
                            if (ImGui::DragFloat(label, &v, vel, min, max))
                                instVal = v;

                            if (instVal.has_value() &&
                                ImGui::SmallButton(("Reset##" + std::string(label)).c_str()))
                                instVal.reset();

                            ImGui::Spacing();
                        };
                    // ---------- Helper lambda for texture slots ----------
                    auto DrawTextureSlot = [&](const char* label,
                        bool srgb,
                        bool& useMap,
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

                            // Checkbox
                            ImGui::Checkbox(("Use##" + std::string(label)).c_str(), &useMap);

                            ImGui::BeginDisabled(!useMap);

                            ImGui::Text(label);
                            ImGui::Button(
                                (texName + std::string("##") + label).c_str(),
                                { ImGui::GetContentRegionAvail().x, 28 }
                            );

                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload =
                                    ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                                {
                                    const TextureDragPayload* data =
                                        static_cast<const TextureDragPayload*>(payload->Data);

                                    if (!assetManager.getTexture(data->id)) {
                                        

                                        assetManager.requestTextureLoad(data->id, data->path, srgb);

                                    }

                                    instSlot = data->id;
                                    useMap = true; // auto-enable when dropped
                                }
                                ImGui::EndDragDropTarget();
                            }

                            if (instSlot.has_value() &&
                                ImGui::SmallButton(("Reset##" + std::string(label)).c_str()))
                                instSlot.reset();

                            ImGui::EndDisabled();
                            ImGui::Spacing();
                        };


                    // ---------- Texture Maps ----------
                    if (baseMat)
                    {
                        DrawTextureSlot("Albedo Map", true,
                            inst.use_map_albedo, inst.map_albedo, baseMat->map_albedo);

                        DrawVec3("Albedo ", inst.albedo, baseMat->albedo);


                        DrawTextureSlot("Ambient Occlusion Map", false,
                            inst.use_map_ao, inst.map_ao, baseMat->map_ao);

                        DrawFloat("Ambient Occlusion",
                            inst.ao,
                            baseMat->ao,
                            0.01f, 0.0f, 1.0f);

                        DrawTextureSlot("Metallic Map", false,
                            inst.use_map_metallic, inst.map_metallic, baseMat->map_metallic);

                        DrawFloat("Metallic",
                            inst.metallic,
                            baseMat->metallic,
                            0.01f, 0.0f, 1.0f);

                        DrawTextureSlot("Normal Map", false,
                            inst.use_map_normal, inst.map_normal, baseMat->map_normal);

                        DrawFloat("Normal Strength",
                            inst.normalStrength,
                            baseMat->normalStrength,
                            0.1f, -100.0f, 100.0f);

                        DrawTextureSlot("Roughness Map", false,
                            inst.use_map_roughness, inst.map_roughness, baseMat->map_roughness);

                        DrawFloat("Roughness",
                            inst.roughness,
                            baseMat->roughness,
                            0.01f, 0.0f, 1.0f);

                        DrawTextureSlot("Metallic Roughness Map", false,
                            inst.use_map_metallicRoughness, inst.map_metallicRoughness, baseMat->map_metallicRoughness);
                    }


                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        else {
                    
            ImGui::PopID();
            continue;
        }
        ImGui::PopID();
}    
    
    
}
