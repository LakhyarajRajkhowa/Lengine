#include "Renderer.h"

#include "../logging/LogBuffer.h"
using namespace Lengine;


ResolvedMaterial Renderer::resolveMaterial(
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

void Renderer::bindCameraUniforms(
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

void Renderer::bindShadowMapUniforms(
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
void Renderer::bindPointShadowUniforms(
    GLSLProgram& shader,
    ShadowCubeMap& shadowCubeMap,
    Light& light
) {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap.getDepthCubeMap());
    shader.setInt("shadowCubeMap", 6);

    shader.setFloat("farPlane", shadowCubeMap.getFarPlane());
}


void Renderer::bindLightUniforms(
    GLSLProgram& shader,
    const Light& light,
    const glm::vec3& sceneAmbient,
    int index
) {
    std::string base = "lights[" + std::to_string(index) + "].";

    shader.setInt(base + "type", (int)light.type);
    shader.setVec3(base + "position", light.position);
    shader.setVec3(base + "direction", light.direction);
    shader.setVec3("sceneAmbient", sceneAmbient);
    shader.setVec3(base + "diffuse", light.diffuse);
    shader.setVec3(base + "specular", light.specular);

    shader.setFloat(base + "constant", light.constant);
    shader.setFloat(base + "linear", light.linear);
    shader.setFloat(base + "quadratic", light.quadratic);

    shader.setFloat(base + "cutOff",
        cos(glm::radians(light.cutOffAngle)));

    shader.setFloat(base + "outerCutOff",
        cos(glm::radians(light.outerCutOffAngle)));
}


void Renderer::bindMaterialUniforms(
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

void Renderer::bindTexture(
    GLSLProgram& shader,
    AssetManager& assetManager,
    const UUID& texID,
    const bool useTexture,
    const char* hasUniform,
    const char* samplerUniform,
    GLenum textureUnit
) {
    if (!useTexture) return;
    glActiveTexture(textureUnit);

    bool hasTexture = texID != UUID::Null;
    shader.setBool(hasUniform, hasTexture);

    if (!hasTexture ) {
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
void Renderer::bindEditorUniforms(
    GLSLProgram& shader,
    const RenderFlags& flags,
    EditorConfig& editorConfig
) {
    shader.setBool("entitySelected", flags.entitySelected);
    shader.setBool("entityEditingMode", editorConfig.editingMode);
}

void Renderer::drawSubMesh(
    SubMesh& sm,
    GLSLProgram& shader,
    bool isHovered
) {
    shader.setBool("isHovered", isHovered);

    if (!sm.isVisible)
        return;

    sm.draw();
}

void Renderer::drawSubMeshGroup(
    Mesh& mesh,
    const std::vector<uint32_t>& subMeshIDs,
    GLSLProgram& shader,
    const std::unordered_set<uint32_t>& hoveredSubMeshes
) {
    for (uint32_t smID : subMeshIDs)
    {
        SubMesh& sm = mesh.subMeshes[smID];

        bool hovered =
            hoveredSubMeshes.find(smID)
            != hoveredSubMeshes.end();

        drawSubMesh(sm, shader, hovered);
    }
}
void Renderer::drawMeshAllSubMeshes(
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


void Renderer::renderScene(
    Scene& activeScene,
    EditorConfig& editorConfig,
    ShadowMap& shadowMap,
    ShadowCubeMap& shadowCubeMap
    ) {

    const auto& entities = activeScene.getEntities();
    auto& lights = activeScene.getLights();
    Light& mainDirectionalLight = activeScene.getMainDirectionalLight();
    Light& mainPointLight = activeScene.getMainPointLight();

    const glm::vec3& sceneAmbient = activeScene.getAmbientLighting();
    collectLights(lights, entities);

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


        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
            if (!subMeshes.size()) continue;
            if (entity->getMaterialIndexInstIDs().empty()) {
                activeScene.assignDefaultMaterials(entity, mesh);
            }
            UUID instID = entity->getMaterialIndexInstIDs().at(matIndex);
            const MaterialInstance& inst = activeScene.getMaterialInstance(instID);

            Material* baseMaterial = assetManager.getMaterial(inst.baseMaterial);
            if (!baseMaterial) continue;


            GLSLProgram* shader = baseMaterial->getShader();
            if (!shader) continue;

            shader->use();
            bindShadowMapUniforms(*shader, shadowMap, mainDirectionalLight);
            bindPointShadowUniforms(*shader, shadowCubeMap, mainPointLight);
            ResolvedMaterial finalMat = resolveMaterial(*baseMaterial, inst);

            bindCameraUniforms(*shader, model, camera);

            shader->setInt("lightCount", (int)lights.size());

            for (int i = 0; i < lights.size(); i++) {
                bindLightUniforms(*shader, lights[i], sceneAmbient, i);
            }

            bindMaterialUniforms(*shader, finalMat);
            bindTexture(
                *shader,
                assetManager,
                finalMat.map_Kd,
                inst.use_map_kd,
                "material.hasDiffuseMap",
                "material.diffuseMap",
                GL_TEXTURE0
            );

            bindTexture(
                *shader,
                assetManager,
                finalMat.map_Ks,
                inst.use_map_ks,
                "material.hasSpecularMap",
                "material.specularMap",
                GL_TEXTURE1
            );
            bindTexture(
                *shader,
                assetManager,
                finalMat.map_bump,
                inst.use_map_bump,
                "material.hasNormalMap",
                "material.normalMap",
                GL_TEXTURE2
            );
            bindEditorUniforms(
                *shader,
                flags,
                editorConfig
            );
            drawSubMeshGroup(
                *mesh,
                subMeshes,
                *shader,
                entity->hoveredSubMeshes
            );

            shader->unuse();
        }

    }
}

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

void Renderer::collectLights(std::vector<Light>& lights, const std::vector<std::unique_ptr<Entity>>& entities)
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

        lights.push_back(gpuLight);
    }
}

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