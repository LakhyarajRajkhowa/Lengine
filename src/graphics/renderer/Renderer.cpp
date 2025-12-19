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

void Renderer::bindLightUniforms(
    GLSLProgram& shader,
    const Light& light
) {
    shader.setInt("light.type", (int)light.type);
    shader.setVec3("light.position", light.position);
    shader.setVec3("light.direction", light.direction);
    shader.setVec3("light.ambient", light.ambient);
    shader.setVec3("light.diffuse", light.diffuse);
    shader.setVec3("light.specular", light.specular);
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
    const char* hasUniform,
    const char* samplerUniform,
    GLenum textureUnit
) {
    bool hasTexture = texID != UUID::Null;
    shader.setBool(hasUniform, hasTexture);

    if (!hasTexture) return;

    GLTexture* tex = assetManager.getTexture(texID);
    if (!tex) return;

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    shader.setInt(samplerUniform, textureUnit - GL_TEXTURE0);
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


void Renderer::renderScene( Scene& scene, Camera3d& camera, AssetManager& assetManager ) {
    const auto& entities = scene.getEntities(); 
    for (const auto& entityPtr : entities) {
       
        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible) continue;

        glm::mat4 model = entity->getTransformMatrix();

        Mesh* mesh = assetManager.getMesh(entity->getMeshID());
        if (!mesh) continue;
       

        for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
            if (!subMeshes.size()) continue;
            if (entity->getMaterialIndexUUIDs().empty()) {
                scene.assignDefaultMaterials(entity, mesh);
            }
            UUID instID = entity->getMaterialIndexUUIDs().at(matIndex);
            const MaterialInstance& inst = scene.getMaterialInstance(instID);

            Material* baseMaterial = assetManager.getMaterial(inst.baseMaterial);
            //Material* baseMaterial = assetManager.getMaterial(entity->materialState.baseMaterials[matIndex]);
            if (!baseMaterial) continue;


            GLSLProgram* shader = baseMaterial->getShader();
            if (!shader) continue;

            shader->use();


            ResolvedMaterial finalMat = resolveMaterial(*baseMaterial, inst);

            bindCameraUniforms(*shader, model, camera);

            bindLightUniforms(*shader, light);
            bindMaterialUniforms(*shader, finalMat);
            bindTexture(
                *shader,
                assetManager,
                finalMat.map_Kd,
                "material.hasDiffuseMap",
                "material.diffuseMap",
                GL_TEXTURE0
            );

            bindTexture(
                *shader,
                assetManager,
                finalMat.map_Ks,
                "material.hasSpecularMap",
                "material.specularMap",
                GL_TEXTURE1
            );
            bindTexture(
                *shader,
                assetManager,
                finalMat.map_bump,
                "material.hasNormalMap",
                "material.normalMap",
                GL_TEXTURE2
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
