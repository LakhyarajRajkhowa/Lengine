#include "InspectorPanel.h"

using namespace Lengine;

InspectorPanel::InspectorPanel(
    SceneManager& scnMgr,
    AssetManager& asstMgr
) :
    sceneManager(scnMgr),
    assets(asstMgr)
{
}





void InspectorPanel::OnImGuiRender() {
    ImGui::Begin("Inspector");

    s_IsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    ImGui::Separator();
    
    if (EditorSelection::HasAsset()) {
        DrawAssetInspector(EditorSelection::GetAsset());
    }
    else if (EditorSelection::HasEntity()) {
        DrawEntityInspector(EditorSelection::GetEntity());
    }

    ImGui::End();

    HandleAssetEditorClear();
    DrawSaveOrCancelPopup();

}

void InspectorPanel::DrawAssetInspector(const std::pair<UUID, AssetType>& asset) {
    if (asset.second == AssetType::Material) {
        DrawMaterialEditor(asset.first);
    }
}



void InspectorPanel::DrawMaterialEditor(const UUID& id)
{
    Material* mat = assets.GetMaterial(id);
    if (!mat)
    {
        ImGui::TextDisabled("Material not loaded");
        ImGui::Separator();

        if (ImGui::Button("Load")) {
            assets.LoadMaterial(id);
        }
        return;
    }

    ImGui::Separator();
    ImGui::Text("PBR Material");
    ImGui::Spacing();

    // ---------- Helpers ----------
    auto DrawVec3 = [&](const char* label, glm::vec3& value)
        {
            if (ImGui::ColorEdit3(label, glm::value_ptr(value)))
            {
                mat->localDirty = true;
            }
            ImGui::Spacing();
        };

    auto DrawFloat = [&](const char* label,
        float& value,
        float speed,
        float min,
        float max)
        {
            if (ImGui::DragFloat(label, &value, speed, min, max))
            {
                mat->localDirty = true;
            }
            ImGui::Spacing();
        };

    auto DrawTextureSlot = [&](const char* label,
        TextureMapType textMapType,
        bool srgb,
        UUID& slot)
        {
            std::string texName = "None";

            if (slot != UUID::Null)
            {
                texName = assets.GetAssetMetaData(slot)->name;
            }

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

                    if (!assets.getTexture(data->id))
                    {
                        assets.RequestTextureLoad(data->id, mat->id, textMapType, srgb);
                    }

                    mat->localDirty = true;
                }
                ImGui::EndDragDropTarget();
            }

            if (slot != UUID::Null &&
                ImGui::SmallButton(("Clear##" + std::string(label)).c_str()))
            {
                slot = UUID::Null;
                mat->localDirty = true;
            }

            ImGui::Spacing();
        };

    // ---------- Base Parameters ----------
    DrawVec3("Albedo", mat->albedo);

    DrawFloat("Ambient Occlusion",
        mat->ao,
        0.01f, 0.0f, 1.0f);

    DrawFloat("Metallic",
        mat->metallic,
        0.01f, 0.0f, 1.0f);

    DrawFloat("Roughness",
        mat->roughness,
        0.01f, 0.0f, 1.0f);

    DrawFloat("Normal Strength",
        mat->normalStrength,
        0.1f, -100.0f, 100.0f);

    ImGui::Separator();

    // ---------- Texture Maps ----------
    DrawTextureSlot("Albedo Map", TextureMapType::Albedo, true, mat->map_albedo);
    DrawTextureSlot("Normal Map", TextureMapType::Normal, false, mat->map_normal);
    DrawTextureSlot("Ambient Occlusion Map", TextureMapType::AmbientOcclusion, false, mat->map_ao);
    DrawTextureSlot("Metallic Map", TextureMapType::Metallic, false, mat->map_metallic);
    DrawTextureSlot("Roughness Map", TextureMapType::Roughness, false, mat->map_roughness);
    DrawTextureSlot("Metallic Roughness Map", TextureMapType::MetallicRoughness, false, mat->map_metallicRoughness);

    // ---------- Save indicator ----------
    if (mat->localDirty)
    {
        if (ImGui::Button("Save")) {
            assets.SaveMaterial(id);
            mat->localDirty = false;
            isAssetSaved = true;
        }

        isAssetSaved = false;

    }
}

void InspectorPanel::DrawEntityMaterialEditor(const UUID& entityID)
{
    MeshRenderer& mr = sceneManager.getActiveScene()->MeshRenderers().Get(entityID);
    MaterialInstance& inst = mr.inst;

    if (inst.baseMaterial == UUID::Null)
    {
        ImGui::TextDisabled("No Base Material Assigned");
        return;
    }

    const Material* baseMat = assets.GetMaterial(inst.baseMaterial);
    if (!baseMat)
    {
        ImGui::TextDisabled("Base Material not loaded");
        if (ImGui::Button("Load Base Material"))
            assets.LoadMaterial(inst.baseMaterial);
        return;
    }

    ImGui::Separator();
    ImGui::Text("Material");
    ImGui::Spacing();

    // ------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------

    auto DrawVec3Override = [&](const char* label,
        const glm::vec3& baseValue,
        std::optional<glm::vec3>& overrideValue)
        {
            glm::vec3 value = overrideValue.value_or(baseValue);

            if (ImGui::ColorEdit3(label, glm::value_ptr(value))) {
                overrideValue = value;
                inst.dirty = true;
            }
            ImGui::SameLine();
            if (overrideValue.has_value())
            {
                if (ImGui::SmallButton(("Reset##" + std::string(label)).c_str())) {
                    overrideValue.reset();
                    inst.dirty = true;
                }
            }

            ImGui::Spacing();
        };

    auto DrawFloatOverride = [&](const char* label,
        const float baseValue,
        std::optional<float>& overrideValue,
        float speed,
        float min,
        float max)
        {
            float value = overrideValue.value_or(baseValue);

            if (ImGui::DragFloat(label, &value, speed, min, max)) {
                overrideValue = value;
                inst.dirty = true;

            }
            ImGui::SameLine();
            if (overrideValue.has_value())
            {
                if (ImGui::SmallButton(("Reset##" + std::string(label)).c_str())) {
                    overrideValue.reset();
                    inst.dirty = true;
                }
            }

            ImGui::Spacing();
        };

    auto DrawTextureOverride = [&](const char* label,
        TextureMapType mapType,
        bool srgb,
        const UUID& baseSlot,
        std::optional<UUID>& overrideSlot,
        bool& useMap)
        {
            UUID current = overrideSlot.value_or(baseSlot);
            std::string texName = "None";

            if (current != UUID::Null)
                texName = assets.GetAssetMetaData(current)->name;

            if (ImGui::Checkbox(("Use##" + std::string(label)).c_str(), &useMap))
                inst.dirty = true;

            ImGui::SameLine();
            ImGui::Text(label);

            ImGui::BeginDisabled(!useMap);

            ImGui::Button(
                (texName + "##" + std::string(label)).c_str(),
                { ImGui::GetContentRegionAvail().x, 28 }
            );

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                {
                    const TextureDragPayload* data =
                        static_cast<const TextureDragPayload*>(payload->Data);

                    if (!assets.getTexture(data->id))
                        assets.RequestTextureLoad_inst(data->id, entityID, mapType, srgb);

                    overrideSlot = data->id;
                    inst.dirty = true;
                }
                ImGui::EndDragDropTarget();
            }

            if (overrideSlot.has_value())
            {
                if (ImGui::SmallButton(("Reset##" + std::string(label)).c_str()))
                {
                    overrideSlot.reset();
                    inst.dirty = true;
                }
            }

            ImGui::EndDisabled();
            ImGui::Spacing();
        };


    // ------------------------------------------------------------
    // Base parameters
    // ------------------------------------------------------------

    DrawVec3Override("Albedo",
        baseMat->albedo,
        inst.albedo);

    DrawFloatOverride("Ambient Occlusion",
        baseMat->ao,
        inst.ao,
        0.01f, 0.0f, 1.0f);

    DrawFloatOverride("Metallic",
        baseMat->metallic,
        inst.metallic,
        0.01f, 0.0f, 1.0f);

    DrawFloatOverride("Roughness",
        baseMat->roughness,
        inst.roughness,
        0.01f, 0.0f, 1.0f);

    DrawFloatOverride("Normal Strength",
        baseMat->normalStrength,
        inst.normalStrength,
        0.1f, -100.0f, 100.0f);

    ImGui::Separator();

    // ------------------------------------------------------------
    // Texture maps
    // ------------------------------------------------------------

    DrawTextureOverride("Albedo Map",
        TextureMapType::Albedo,
        true,
        baseMat->map_albedo,
        inst.map_albedo,
        inst.use_map_albedo);

    DrawTextureOverride("Normal Map",
        TextureMapType::Normal,
        false,
        baseMat->map_normal,
        inst.map_normal,
        inst.use_map_normal);

    DrawTextureOverride("AO Map",
        TextureMapType::AmbientOcclusion,
        false,
        baseMat->map_ao,
        inst.map_ao,
        inst.use_map_ao);

    DrawTextureOverride("Metallic Map",
        TextureMapType::Metallic,
        false,
        baseMat->map_metallic,
        inst.map_metallic,
        inst.use_map_metallic);

    DrawTextureOverride("Roughness Map",
        TextureMapType::Roughness,
        false,
        baseMat->map_roughness,
        inst.map_roughness,
        inst.use_map_roughness);

    DrawTextureOverride("Metallic Roughness Map",
        TextureMapType::MetallicRoughness,
        false,
        baseMat->map_metallicRoughness,
        inst.map_metallicRoughness,
        inst.use_map_metallicRoughness);
}


// TODO : break down this func into parts
void InspectorPanel::DrawEntityInspector(const UUID& entityID)
{
    ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x, 30 };

  
    if (entityID == UUID::Null) {
        ImGui::Text("No entity selected.");
        return;
    }

    Scene* scene = sceneManager.getActiveScene();
    Entity* entity = scene->getEntityByID(entityID);

    if (!entity) {
        ImGui::Text("No entity selected.");
        return;
    }
    
    // ---------------- NAME ----------------
    char buffer[256] = {};
    strcpy_s(buffer, sizeof(buffer), entity->getName().c_str());


    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
        if (buffer[0] != '\0') {
            entity->setName(buffer);
        }
    }
    ImGui::Spacing();

    

    if (scene->Transforms().Has(entity->getID()))
    {
        auto& tr = scene->Transforms().Get(entity->getID());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::Separator();
        ImGui::Spacing();

        // ---------- POSITION ----------
        ImGui::Text("Position");
        ImGui::SameLine();

        if (ImGui::Button("Reset##pos"))
        {
            tr.localPosition = { 0.0f, 0.0f, 0.0f };
            tr.localDirty = true;
            tr.worldDirty = true;

            TransformSystem::Dirty = true;
            
        }

        if (ImGui::DragFloat3(
            "##Position",
            glm::value_ptr(tr.localPosition),
            0.1f
        ))
        {
            tr.localDirty = true;
            tr.worldDirty = true;

            TransformSystem::Dirty = true;


        }

        // ---------- ROTATION (Degrees UI) ----------
        ImGui::Text("Rotation");
        ImGui::SameLine();

        if (ImGui::Button("Reset##rot"))
        {
            tr.localRotation = glm::vec3(0.0f);
            tr.localDirty = true;
            tr.worldDirty = true;

            TransformSystem::Dirty = true;


        }

        glm::vec3 rotationDeg = glm::degrees(tr.localRotation);

        if (ImGui::DragFloat3(
            "##Rotation",
            glm::value_ptr(rotationDeg),
            0.5f
        ))
        {
            tr.localRotation = glm::radians(rotationDeg);
            tr.localDirty = true;
            tr.worldDirty = true;

            TransformSystem::Dirty = true;


        }

        // ---------- SCALE ----------
        ImGui::Text("Scale");
        ImGui::SameLine();

        if (ImGui::Button("Reset##scale"))
        {
            tr.localScale = { 1.0f, 1.0f, 1.0f };
            tr.localDirty = true;
            tr.worldDirty = true;

            TransformSystem::Dirty = true;


        }

        bool& uniformScale = inspectorState.uniformScale;
        ImGui::Checkbox("Uniform", &uniformScale);

        if (uniformScale)
        {
            float s = tr.localScale.x;
            if (ImGui::DragFloat("##UniformScale", &s, 0.05f))
            {
                tr.localScale = { s, s, s };
                tr.localDirty = true;
                tr.worldDirty = true;

                TransformSystem::Dirty = true;


            }
        }
        else
        {
            if (ImGui::DragFloat3(
                "##Scale",
                glm::value_ptr(tr.localScale),
                0.05f
            ))
            {
                tr.localDirty = true;

                TransformSystem::Dirty = true;

            }
        }
    }
    else
    {
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Add Transform Component", buttonSize))
        {
            scene->Transforms().Add(entity->getID());
        }

        ImGui::Separator();
        ImGui::Spacing();
    }




    // ---------------- MESH BLOCK ----------------

    if (scene->MeshFilters().Has(entityID)) {
        MeshFilter& mf = scene->MeshFilters().Get(entity->getID());

        ImGui::Separator();
        ImGui::Text("Mesh Filter");
        ImGui::Separator();
        ImGui::Spacing();
        // --- Draw a centered box ---
        ImGui::BeginGroup();

        // Mesh name string
        std::string submeshName = "None";
        const UUID& submeshID = mf.submeshID;

        if (!submeshID.isNull() && !mf.HasPendingSubmesh() ) {
            const AssetMetadata* submeshMetaData = assets.GetAssetMetaData(mf.submeshID);
            submeshName = submeshMetaData ? submeshMetaData->name : "Invalid Submesh";
        }
        ImGui::Button((submeshName + "##Submesh").c_str(), buttonSize);

        // --- Drag Drop Target ---
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SUBMESH_ASSET"))
            {
                const MeshDragPayload* data = (const MeshDragPayload*)payload->Data;

                UUID droppedID = data->id;

                if (assets.assetStates[droppedID] == AssetState::Loaded) {
                    mf.submeshID = droppedID;
                }
                else {
                    assets.RequestSubmeshLoad(droppedID, entity->getID());
                    mf.RequestSubmesh(droppedID);
                }

            }
            ImGui::EndDragDropTarget();
        }

        ImGui::EndGroup();
    }
    else {
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Add Mesh Filter Component", buttonSize)) {
            scene->MeshFilters().Add(entity->getID());
        }

        ImGui::Separator();
        ImGui::Spacing();

    }


    if (scene->MeshRenderers().Has(entityID)) {
        MeshRenderer& mr = scene->MeshRenderers().Get(entity->getID());



        ImGui::Separator();
        ImGui::Text("Mesh Renderer");
        ImGui::Separator();
        ImGui::Spacing();
        // --- Draw a centered box ---
        ImGui::BeginGroup();

        // Mesh name string
        std::string materialName = "None";
        UUID materialID = mr.inst.baseMaterial;

        if (!materialID.isNull()) {
            const AssetMetadata* materialMetaData = assets.GetAssetMetaData(materialID);
            materialName = materialMetaData ? materialMetaData->name : "Invalid Material";
        }
        ImGui::Button((materialName + "##Material").c_str(), buttonSize);

        // --- Drag Drop Target ---
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PBRMATERIAL_ASSET"))
            {
                const MaterialDragPayload* data = (const MaterialDragPayload*)payload->Data;

                UUID droppedID = data->id;

                if (assets.GetMaterial(droppedID)) {
                    mr.inst.baseMaterial = droppedID;
                    mr.inst.dirty = true;
                }
                else {
                    if (assets.LoadMaterial(droppedID)) {
                        mr.inst.baseMaterial = droppedID;
                        mr.inst.dirty = true;
                    }


                }

            }
            ImGui::EndDragDropTarget();
        }

        DrawEntityMaterialEditor(entityID);

        ImGui::EndGroup();
    }
    else {
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Add Mesh Renderer Component", buttonSize)) {
            scene->MeshRenderers().Add(entityID);
        }

        ImGui::Separator();
        ImGui::Spacing();

    }

   
    if (scene->Lights().Has(entity->getID()))
    {
        auto& light = scene->Lights().Get(entity->getID());

        ImGui::Separator();
        ImGui::Text("Lighting");
        ImGui::Separator();
        ImGui::Spacing();

        // ---------------- Light Type ----------------
        const char* lightTypes[] = { "Directional", "Point", "Spotlight" };
        int currentType = static_cast<int>(light.type);

        if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes)))
        {
            light.type = static_cast<LightType>(currentType);
        }

        ImGui::Spacing();

        // ---------------- Color ----------------
        ImGui::Text("Color");
        ImGui::SameLine();
        ImGui::ColorEdit3(
            "##LightColor",
            glm::value_ptr(light.color)
        );

        // ---------------- Intensity ----------------
        ImGui::Spacing();
        ImGui::DragFloat(
            "Intensity",
            &light.intensity,
            0.1f,
            0.0f,
            100000.0f,
            "%.2f"
        );

        // ---------------- Cast Shadows ----------------
        ImGui::Spacing();

        if (ImGui::Checkbox("Cast Shadows", &light.castShadow))
        {
            if (light.castShadow) // turned ON
            {
                if (light.type == LightType::Directional)
                    scene->Lights().SetDirectionalShadowCaster(entityID);
                else
                    scene->Lights().SetPointShadowCaster(entityID);
            }
            else // turned OFF
            {
                if (light.type == LightType::Directional)
                    scene->Lights().ClearDirectionalShadowCaster(entityID);
                else
                    scene->Lights().ClearPointShadowCaster(entityID);
            }
        }


        // ---------------- Range ----------------
        if (light.type == LightType::Point || light.type == LightType::Spotlight)
        {
            ImGui::Spacing();
            ImGui::DragFloat(
                "Range",
                &light.range,
                0.1f,
                0.1f,
                1000.0f,
                "%.2f"
            );
        }

        // ---------------- Spotlight Angles ----------------
        if (light.type == LightType::Spotlight)
        {
            ImGui::Spacing();
            ImGui::DragFloat(
                "Inner Angle",
                &light.innerAngle,
                0.1f,
                0.0f,
                light.outerAngle - 0.1f,
                "%.1f deg"
            );

            ImGui::DragFloat(
                "Outer Angle",
                &light.outerAngle,
                0.1f,
                light.innerAngle + 0.1f,
                90.0f,
                "%.1f deg"
            );
        }

        ImGui::Spacing();
        ImGui::Separator();

        // ---------------- Remove ----------------
        if (ImGui::Button("Remove Light Component"))
        {
            scene->Lights().Remove(entity->getID());
        }

        ImGui::Spacing();
    }
    else
    {
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Add Light Component", buttonSize))
        {
            scene->Lights().Add(entity->getID());
        }

        ImGui::Separator();
        ImGui::Spacing();
    }

       
}

 void InspectorPanel::DrawSaveOrCancelPopup()
{
     if (EditorSelection::GetAsset().first == UUID::Null) return;

    if (s_OpenSaveOrCancelPopup)
    {
        ImGui::OpenPopup("Unsaved Material");
        s_OpenSaveOrCancelPopup = false;
    }

    if (ImGui::BeginPopupModal(
        "Unsaved Material",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("You have unsaved changes.");
        ImGui::Text("Do you want to save before closing?");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            // for now assume only material asset needs saving
            const UUID matID = EditorSelection::GetAsset().first;
            Material* mat = assets.GetMaterial(matID);
            mat->localDirty = false;

            assets.SaveMaterial(matID);
            
            isAssetSaved = true;        
            EditorSelection::ClearAssetSelection();

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();


        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

 void InspectorPanel::HandleAssetEditorClear()
 {
     if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
     {
         // If mouse is NOT over inspector window
         if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) ||
             !s_IsHovered)
         {
             // And not clicking on any UI item
             if (!ImGui::IsAnyItemHovered())
             {
                 if (!isAssetSaved) {
                     s_OpenSaveOrCancelPopup = true;
                 }
                 else {
                     EditorSelection::ClearAssetSelection();

                 }
             }
         }
     }
 }

