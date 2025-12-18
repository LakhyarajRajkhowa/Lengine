#include "Renderer.h"

#include "../logging/LogBuffer.h"
using namespace Lengine;


void Renderer::renderScene( Scene& scene, Camera3d& camera, AssetManager& assetManager ) {
    const auto& entities = scene.getEntities(); 
   // const Entity* sun = scene.getEntityByName("sun");

   // if (!sun) lightPos = glm::vec3(10, 10, 10);
   // else lightPos = sun->getTransform().position;

    for (const auto& entityPtr : entities) {
       
        Entity* entity = entityPtr.get();
        if (!entity || !entity->isVisible) continue;

        glm::mat4 model = entity->getTransformMatrix();

        Mesh* mesh = assetManager.getMesh(entity->getMeshID());
        if (!mesh) continue;
       

            for (auto& [matIndex, subMeshes] : mesh->materialGroups) {
                if (entity->getMaterialIndexUUIDs().empty()) {
                    scene.assignDefaultMaterials(entity, mesh);
                }
                UUID instID = entity->getMaterialIndexUUIDs().at(matIndex);
                const MaterialInstance& inst = scene.getMaterialInstance(instID);

                Material* baseMaterial = assetManager.getMaterial(inst.baseMaterial);
                if (!baseMaterial) continue;


                GLSLProgram* shader = baseMaterial->getShader();
                if (!shader) continue;

                ResolvedMaterial finalMat{};
                finalMat.Ka = inst.Ka.value_or(baseMaterial->Ka);
                finalMat.Kd = inst.Kd.value_or(baseMaterial->Kd);
                finalMat.Ks = inst.Ks.value_or(baseMaterial->Ks);
                finalMat.Ke = inst.Ke.value_or(baseMaterial->Ke);
                finalMat.Ns = inst.Ns.value_or(baseMaterial->Ns);
                finalMat.map_Kd = inst.map_kd.value_or(baseMaterial->map_Kd);
                finalMat.map_Ks = inst.map_ks.value_or(baseMaterial->map_Ks);

                shader->use();
                shader->setMat4("model", model);
                shader->setMat4("view", camera.getViewMatrix());
                shader->setMat4("projection", camera.getProjectionMatrix());
                shader->setVec3("cameraPos", camera.getCameraPosition());
                shader->setVec3("viewPos", camera.getCameraPosition());


                shader->setInt("light.type", (int)light.type);
                shader->setVec3("light.position", light.position);
                shader->setVec3("light.direction", light.direction);
                shader->setVec3("light.ambient", light.ambient);
                shader->setVec3("light.diffuse", light.diffuse);
                shader->setVec3("light.specular", light.specular);


                shader->setVec3("material.Ka", finalMat.Ka);
                shader->setVec3("material.Kd", finalMat.Kd);
                shader->setVec3("material.Ks", finalMat.Ks);
                shader->setVec3("material.Ke", finalMat.Ke);
                shader->setFloat("material.Ns", finalMat.Ns);

                // Diffuse map
                bool hasDiffuseMap =
                    inst.map_kd.has_value() ||
                    (baseMaterial->map_Kd != UUID::Null);
                shader->setBool("material.hasDiffuseMap", hasDiffuseMap);
                if (hasDiffuseMap)
                {
                    GLTexture* diffuseTex = nullptr;

                    if (inst.map_kd.has_value())
                    {
                        diffuseTex = assetManager.getTexture(inst.map_kd.value());
                    }
                    else
                    {
                        diffuseTex = assetManager.getTexture(baseMaterial->map_Kd);
                    }
                    if (diffuseTex)
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, diffuseTex->id);
                        shader->setInt("material.diffuseMap", 0);
                    }
                }

                // Specular map
                bool hasSpecularMap =
                    inst.map_ks.has_value() ||
                    (baseMaterial->map_Ks != UUID::Null);
                shader->setBool("material.hasSpecularMap", hasSpecularMap);
                if (hasSpecularMap)
                {
                    GLTexture* specularTex = nullptr;

                    if (inst.map_ks.has_value())
                    {
                        specularTex = assetManager.getTexture(inst.map_ks.value());
                    }
                    else
                    {
                        specularTex = assetManager.getTexture(baseMaterial->map_Ks);
                    }
                    if (specularTex)
                    {
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, specularTex->id);
                        shader->setInt("material.specularMap", 1);
                    }
                }


                for (auto& smID : subMeshes) {
                    SubMesh& sm = mesh->subMeshes[smID];
                    bool hovered =
                        entity->hoveredSubMeshes.find(smID)
                        != entity->hoveredSubMeshes.end();

                    shader->setBool("isHovered", hovered);
                    if(sm.isVisible)
                        sm.draw();
                }
                
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
