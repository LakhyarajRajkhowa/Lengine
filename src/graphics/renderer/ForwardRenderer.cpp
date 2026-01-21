#include "ForwardRenderer.h"

#include "../logging/LogBuffer.h"
using namespace Lengine;



void ForwardRenderer::collectLights(std::vector<Light>& lights, const std::vector<std::unique_ptr<Entity>>& entities)
{
    lights.clear();

    for (const auto& ePtr : entities) {
        const Entity& e = *ePtr;

        if (e.getType() != EntityType::Light || !e.hasLight())
            continue;

        const Light& src = e.getLight();

        Light gpuLight{};
        gpuLight.type = src.type;

        gpuLight.position = e.getTransform().position;
        glm::quat q = glm::quat(e.getTransform().rotation); // radians
        gpuLight.direction = glm::normalize(q * glm::vec3(0.0f, 0.0f, -1.0f));


        gpuLight.ambient = src.ambient;
        gpuLight.diffuse = src.diffuse;
        gpuLight.specular = src.specular;

        gpuLight.constant = src.constant;
        gpuLight.linear = src.linear;
        gpuLight.quadratic = src.quadratic;

        gpuLight.cutOffAngle = src.cutOffAngle;
        gpuLight.outerCutOffAngle = src.outerCutOffAngle;

        gpuLight.intensity = src.intensity;

        gpuLight.castShadow = src.castShadow;
        lights.push_back(gpuLight);
    }
}


ResolvedMaterial ForwardRenderer::resolveMaterial(
    const Material& baseMaterial,
    const MaterialInstance& inst
) {
    ResolvedMaterial mat{};
    mat.Ka = inst.Ka.value_or(baseMaterial.Ka);
    mat.Kd = inst.Kd.value_or(baseMaterial.Kd);
    mat.Ks = inst.Ks.value_or(baseMaterial.Ks);
    mat.Ke = inst.Ke.value_or(baseMaterial.Ke);
    mat.Ns = inst.Ns.value_or(baseMaterial.Ns);
    mat.map_Kd = inst.map_kd.value_or(baseMaterial.map_Kd);
    mat.map_Ks = inst.map_ks.value_or(baseMaterial.map_Ks);
    mat.map_bump = inst.map_bump.value_or(baseMaterial.map_bump);

    mat.normalStrength = inst.normalStrength.value_or(baseMaterial.normalStrength);
    return mat;
}


ResolvedPBRMaterial ForwardRenderer::resolvePBRMaterial(
    const PBRMaterial& baseMaterial,
    const PBRMaterialInstance& inst
) {
    ResolvedPBRMaterial mat{};

    // Scalars
    mat.albedo = inst.albedo.value_or(baseMaterial.albedo);
    mat.metallic = inst.metallic.value_or(baseMaterial.metallic);
    mat.roughness = inst.roughness.value_or(baseMaterial.roughness);
    mat.ao = inst.ao.value_or(baseMaterial.ao);

    // Texture maps
    mat.map_albedo = inst.map_albedo.value_or(baseMaterial.map_albedo);
    mat.map_normal = inst.map_normal.value_or(baseMaterial.map_normal);
    mat.map_metallic = inst.map_metallic.value_or(baseMaterial.map_metallic);
    mat.map_roughness = inst.map_roughness.value_or(baseMaterial.map_roughness);
    mat.map_ao = inst.map_ao.value_or(baseMaterial.map_ao);
    mat.map_metallicRoughness = inst.map_metallicRoughness.value_or(baseMaterial.map_metallicRoughness);

    mat.normalStrength =
        inst.normalStrength.value_or(baseMaterial.normalStrength);

    return mat;
}


void ForwardRenderer::bindCameraUniforms(
    GLSLProgram& shader,
    const glm::mat4& model,
    Camera3d& camera
) {

    shader.setMat4("model", model); 
    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", camera.getProjectionMatrix());
    shader.setVec3("cameraPos", camera.getCameraPosition());
    shader.setVec3("viewPos", camera.getCameraPosition());

}

void ForwardRenderer::bindShadowMapUniforms(
    GLSLProgram& shader,
    ShadowMap& shadowMap,
    Light& mainDirectionalLight
) {
    shader.setMat4(
        "lightSpaceMatrix",
        mainDirectionalLight.getSpaceMatrix()
    );

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());
    shader.setInt("shadowMap", 5);
}
void ForwardRenderer::bindPointShadowUniforms(
    GLSLProgram& shader,
    ShadowCubeMap& shadowCubeMap,
    Light& light
) {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap.getDepthCubeMap());
    shader.setInt("shadowCubeMap", 6);

    shader.setFloat("farPlane", shadowCubeMap.getFarPlane());
}


void ForwardRenderer::bindLightUniforms(
    GLSLProgram& shader,
    const Light& light,
    const glm::vec3& sceneAmbient,
    int index
) {
    std::string base = "lights[" + std::to_string(index) + "].";

    shader.setInt(base + "type", (int)light.type);
    shader.setBool(base + "castShadow", light.castShadow);
    shader.setVec3(base + "position", light.position);
    shader.setVec3(base + "direction", light.direction);
    shader.setVec3("sceneAmbient", sceneAmbient);
    shader.setVec3(base + "diffuse", light.diffuse);
    shader.setVec3(base + "specular", light.specular);
    shader.setFloat(base + "intensity", light.intensity);
    shader.setFloat(base + "constant", light.constant);
    shader.setFloat(base + "linear", light.linear);
    shader.setFloat(base + "quadratic", light.quadratic);

    shader.setFloat(base + "cutOff",
        cos(glm::radians(light.cutOffAngle)));

    shader.setFloat(base + "outerCutOff",
        cos(glm::radians(light.outerCutOffAngle)));


}


void ForwardRenderer::bindPBRLights(
    GLSLProgram& shader,
    const std::vector<Light>& lights
) {
    int count = (int)lights.size();

    shader.setInt("lightCount", count);

    for (int i = 0; i < count; i++) {
        shader.setVec3(
            "lightPositions[" + std::to_string(i) + "]",
            lights[i].position
        );

        shader.setVec3(
            "lightColors[" + std::to_string(i) + "]",
            lights[i].diffuse * lights[i].intensity
        );
    }
}

void ForwardRenderer::bindPBRMaterial(
    GLSLProgram& shader,
    const ResolvedPBRMaterial& mat
) {
    shader.setVec3("material.albedo", mat.albedo);
    shader.setFloat("material.metallic", mat.metallic);
    shader.setFloat("material.roughness", mat.roughness);
    shader.setFloat("material.ao", mat.ao);
    shader.setFloat("material.normalStrength", mat.normalStrength);
}



void ForwardRenderer::bindMaterialUniforms(
    GLSLProgram& shader,
    const ResolvedMaterial& mat
) {
    shader.setVec3("material.Ka", mat.Ka);
    shader.setVec3("material.Kd", mat.Kd);
    shader.setVec3("material.Ks", mat.Ks);
    shader.setVec3("material.Ke", mat.Ke);
    shader.setFloat("material.Ns", mat.Ns);
    shader.setFloat("material.normalStrength", mat.normalStrength);
}

void ForwardRenderer::bindTexture(
    GLSLProgram& shader,
    AssetManager& assetManager,
    const UUID& texID,
    const bool UseTexture,
    const char* hasUniform,
    const char* samplerUniform,
    GLenum textureUnit
) {
    bool hasTexture = (texID != UUID::Null && UseTexture);
    shader.setBool(hasUniform, hasTexture);

    if (!UseTexture) return;

    glActiveTexture(textureUnit);



    if (!hasTexture) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    GLTexture* tex = assetManager.getTexture(texID);
    if (!tex) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    glBindTexture(GL_TEXTURE_2D, tex->id);

    shader.setInt(samplerUniform, textureUnit - GL_TEXTURE0);
}
void ForwardRenderer::bindEditorUniforms(
    GLSLProgram& shader,
    const RenderFlags& flags,
    const EditorConfig& editorConfig
) {
    shader.setBool("entitySelected", flags.entitySelected);
    shader.setBool("entityEditingMode", editorConfig.editingMode);
}

void ForwardRenderer::drawSubMesh(
    SubMesh& sm,
    GLSLProgram& shader
) {

    if (!sm.isVisible)
        return;

    sm.draw();
}

void ForwardRenderer::drawSubMeshGroup(
    Mesh& mesh,
    const std::vector<uint32_t>& subMeshIDs,
    GLSLProgram& shader
) {
    for (uint32_t smID : subMeshIDs)
    {
        SubMesh& sm = mesh.subMeshes[smID];
        drawSubMesh(sm, shader);
    }
}
void ForwardRenderer::drawMeshAllSubMeshes(
    Mesh& mesh,
    GLSLProgram& shader
)
{

    for (const SubMesh& sm : mesh.subMeshes)
    {
        if (sm.isVisible)
            sm.draw();
    }

}

// TODO : Rendering based on same shaders
// right now i m using one universal defaultShader to render all entities
// although it doesnt gives much fps gain

// TODO : draw call instancing / batching

/*
void ForwardRenderer::RenderScene_phong(
    Scene& activeScene,
    const EditorConfig& editorConfig,
    ShadowMap& shadowMap,
    ShadowCubeMap& shadowCubeMap
) {

    defaultShader = assetManager.getShader("defaultShader.vert");

    const auto& entities = activeScene.getEntities();

    auto& lights = activeScene.getLights();
    Light& mainDirectionalLight = activeScene.getMainDirectionalLight();
    Light& mainPointLight = activeScene.getMainPointLight();

    const glm::vec3& sceneAmbient = activeScene.getAmbientLighting();
    collectLights(lights, entities);

    defaultShader->use();
    bindShadowMapUniforms(*defaultShader, shadowMap, mainDirectionalLight);
    bindPointShadowUniforms(*defaultShader, shadowCubeMap, mainPointLight);
    defaultShader->setInt("lightCount", (int)lights.size());
    for (int i = 0; i < lights.size(); i++) {
        bindLightUniforms(*defaultShader, lights[i], sceneAmbient, i);
    }

    for (const auto& entityPtr : entities) {

        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible) continue;

        glm::mat4 model = entity->getTransformMatrix();
        UUID meshID = entity->getMeshID();
        Mesh* mesh = assetManager.getMesh(meshID);
        if (!mesh || entity->hasPendingMesh()) continue;

        RenderFlags flags;
        flags.entitySelected = entity->isSelected;
        flags.entityDragged = entity->isDragged;

        bindCameraUniforms(*defaultShader, model, camera);


        
        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
            if (!subMeshes.size()) continue;
            if (entity->getMaterialIndexInstIDs().empty()) {
                activeScene.assignDefaultMaterials(entity, mesh);
            }
            UUID instID = entity->getMaterialIndexInstIDs().at(matIndex);
            const MaterialInstance& inst = activeScene.getMaterialInstance(instID);

            Material* baseMaterial = assetManager.getMaterial(inst.baseMaterial);
            if (!baseMaterial) continue;

            
            ResolvedMaterial finalMat = resolveMaterial(*baseMaterial, inst);

            
            bindMaterialUniforms(*defaultShader, finalMat);
            bindTexture(
                *defaultShader,
                assetManager,
                finalMat.map_Kd,
                inst.use_map_kd,
                "material.hasDiffuseMap",
                "material.diffuseMap",
                GL_TEXTURE0
            );

            bindTexture(
                *defaultShader,
                assetManager,
                finalMat.map_Ks,
                inst.use_map_ks,
                "material.hasSpecularMap",
                "material.specularMap",
                GL_TEXTURE1
            );
            bindTexture(
                *defaultShader,
                assetManager,
                finalMat.map_bump,
                inst.use_map_bump,
                "material.hasNormalMap",
                "material.normalMap",
                GL_TEXTURE2
            );
            bindEditorUniforms(
                *defaultShader,
                flags,
                editorConfig
            );
            
            drawSubMeshGroup(
                *mesh,
                subMeshes,
                *defaultShader,
                entity->hoveredSubMeshes
            );
            
          
        }
        

    }

    defaultShader->unuse();
}
*/




void ForwardRenderer::RenderScene_pbr(
    Scene& activeScene,
    const EditorConfig& editorConfig,
    ShadowMap& shadowMap,
    ShadowCubeMap& shadowCubeMap,
    const GLTexture& irradianceMap
) {
    GLSLProgram* pbrShader = assetManager.getShader("pbr.vert");
    pbrShader->use();
    pbrShader->setInt("irradianceMap", 0);

    const auto& entities = activeScene.getEntities();
    auto& meshRenderers = activeScene.MeshRenderers();
    auto& meshFilters = activeScene.MeshFilters();

    auto& lights = activeScene.getLights();

    collectLights(lights, entities);

    // Bind camera once (view & projection are global)
    pbrShader->setMat4("view", camera.getViewMatrix());
    pbrShader->setMat4("projection", camera.getProjectionMatrix());
    pbrShader->setVec3("cameraPos", camera.getCameraPosition());
  
    // bind pre-computed IBL data
    
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap.id);
    pbrShader->setInt("irradianceMap", 10);


    // Bind lights
    bindPBRLights(*pbrShader, lights);


    for (const auto& entityPtr : entities) { 
        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible) continue;

        auto& mr = meshRenderers.Get(entity->getID());
        auto& mf = meshFilters.Get(entity->getID());


        Mesh* mesh = assetManager.getMesh(mf.meshID);
        if (!mesh || mf.HasPendingMesh()) continue;

        glm::mat4 model = entity->getTransformMatrix();
        pbrShader->setMat4("model", model);

        glm::mat3 normalMatrix =
            glm::transpose(glm::inverse(glm::mat3(model)));

       // pbrShader->setMat3("normalMatrix", normalMatrix);

        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {

            if (!subMeshes.size()) continue;


            if (mr.materialIndexToInstID.empty()) {
                activeScene.assignDefaultPBRMaterials(entity->getID(), mesh);
            }

            UUID instID = mr.materialIndexToInstID.at(matIndex);
            PBRMaterialInstance& inst = activeScene.getPbrMaterialInstance(instID);

            PBRMaterial* baseMaterial = assetManager.getPBRMaterial(inst.baseMaterial);
            if (!baseMaterial) continue;


            ResolvedPBRMaterial finalMat = resolvePBRMaterial(*baseMaterial, inst);


            // ---- GPU BIND ----
            bindPBRMaterial(*pbrShader, finalMat);

            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_albedo,
                inst.use_map_albedo,
                "material.hasAlbedoMap",
                "material.albedoMap",
                GL_TEXTURE1
            );


            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_normal,
                inst.use_map_normal,
                "material.hasNormalMap",
                "material.normalMap",
                GL_TEXTURE2
            );
            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_metallicRoughness,
                inst.use_map_metallicRoughness,
                "material.hasMetallicRoughnessMap",
                "material.metallicRoughnessMap",
                GL_TEXTURE3
            );


            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_ao,
                inst.use_map_ao,
                "material.hasAOMap",
                "material.aoMap",
                GL_TEXTURE4
            );
            
            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_metallic,
                inst.use_map_metallic,
                "material.hasMetallicMap",
                "material.metallicMap",
                GL_TEXTURE5
            );

            bindTexture(
                *pbrShader,
                assetManager,
                finalMat.map_roughness,
                inst.use_map_roughness,
                "material.hasRoughnessMap",
                "material.roughnessMap",
                GL_TEXTURE6
            );



            drawSubMeshGroup(
                *mesh,
                subMeshes,
                *pbrShader
            );
        }
    }

    pbrShader->unuse();
}


/*
 
 TODO : SSAO 

void ForwardRenderer::renderSceneDepthNormals(
    Scene& activeScene,
    const EditorConfig& editorConfig
) {
    auto depthNormalShader =
        assetManager.getShader("depthNormalShader.vert");

    depthNormalShader->use();

    // Camera matrices
    depthNormalShader->setMat4("uView", camera.getViewMatrix());
    depthNormalShader->setMat4("uProjection", camera.getProjectionMatrix());

    const auto& entities = activeScene.getEntities();

    for (const auto& entityPtr : entities) {

        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible)
            continue;

        UUID meshID = entity->getMeshID();
        Mesh* mesh = assetManager.getMesh(meshID);
        if (!mesh || entity->hasPendingMesh())
            continue;

        glm::mat4 model = entity->getTransformMatrix();
        depthNormalShader->setMat4("uModel", model);

        // IMPORTANT: no materials, no textures
        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
            if (!subMeshes.size()) continue;

            drawSubMeshGroup(
                *mesh,
                subMeshes,
                *depthNormalShader,
                {} // no hovered submesh highlight
            );
        }
    }

    depthNormalShader->unuse();
}
*/

/*
void Renderer::initOutlineShader() {
    outlineShader.compileShaders(
        Paths::Shaders + "defaultShader.vert",
        Paths::Shaders + "outlineShader.frag"
    );

    outlineShader.linkShaders();
}

void Renderer::renderSelectionOutline(
    Entity& entity,
    Camera3d& camera,
    AssetManager& assetManager
)
{
    if (!entity.isSelected)
        return;

    Mesh* mesh = assetManager.getMesh(entity.getMeshID());
    if (!mesh || entity.hasPendingMesh())
        return;

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    outlineShader.use();

    glm::mat4 outlineModel =
        glm::scale(entity.getTransformMatrix(), glm::vec3(1.03f));

    bindCameraUniforms(outlineShader, outlineModel, camera);
    outlineShader.setVec3("outlineColor", glm::vec3(1.0f, 0.85f, 0.25f));

    drawMeshAllSubMeshes(*mesh, outlineShader);

    outlineShader.unuse();

    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
}
*/


/*
void Renderer::collectRenderData(Scene& scene, Camera3d& camera, AssetManager& assetManager) {

    for (auto& [entityIndex, mr] : scene.getMeshRenderers())
    {

        const Entity* entity = scene.getEntityByIndex(entityIndex);

        glm::mat4 model = entity->getTransformMatrix();

        Mesh* mesh = assetManager.getMesh(mr.meshID);
        if (!mesh) continue;

        for (size_t i = 0; i < mesh->subMeshes.size(); i++)
        {
            RenderKey key;
            key.shader = assetManager.getMaterial(mr.materials[i])->getShader();
            key.materialID = mr.materials[i];
            key.meshID = mr.meshID;
            key.subMeshIndex = i;

            batcher.submit(key, {
                entityIndex,
                model
                });
        }
    }

}

void Renderer::applyMaterialUniforms(GLSLProgram* shader,
    UUID materialID,
    AssetManager& assetManager)
{
    Material* mat = assetManager.getMaterial(materialID);
    if (!mat) return;

    shader->setVec3("material.Ka", mat->Ka);
    shader->setVec3("material.Kd", mat->Kd);
    shader->setVec3("material.Ks", mat->Ks);
    shader->setVec3("material.Ke", mat->Ke);
    shader->setFloat("material.Ns", mat->Ns);
}

void Renderer::flushBatches(Scene& scene,
    Camera3d& camera,
    AssetManager& assetManager)
{
    for (auto& [key, drawList] : batcher.batches)
    {
        GLSLProgram* shader = key.shader;
        if (!shader) continue;

        shader->use();

        // ---- Camera uniforms (once per batch) ----
        shader->setMat4("view", camera.getViewMatrix());
        shader->setMat4("projection", camera.getProjectionMatrix());
        shader->setVec3("cameraPos", camera.getCameraPosition());

        shader->setVec3("lightColor", lightColor);
        shader->setVec3("lightPos", lightPos);

        // ---- Material uniforms (once per batch) ----
        applyMaterialUniforms(shader, key.materialID, assetManager);

        // ---- Geometry ----
        Mesh* mesh = assetManager.getMesh(key.meshID);
        if (!mesh) continue;

        SubMesh& sm = mesh->subMeshes[key.subMeshIndex];

        // ---- Draw all entities in this batch ----
        for (const RenderCommand& cmd : drawList)
        {
            shader->setMat4("model", cmd.model);
            sm.draw();
        }

        shader->unuse();
    }

    batcher.clear(); // IMPORTANT: per-frame
}

void Renderer::renderSceneECS(Scene& scene, Camera3d& camera, AssetManager& assetManager) {

    collectRenderData(scene, camera, assetManager);
    flushBatches(scene, camera, assetManager);

}
*/