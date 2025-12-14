#include "Renderer.h"

#include "../logging/LogBuffer.h"
using namespace Lengine;


void Renderer::renderScene(const Scene& scene, Camera3d& camera, AssetManager& assetManager ) {
    const auto& entities = scene.getEntities();
    const Entity* sun = scene.getEntityByName("sun");
    
    if (!sun) lightPos = glm::vec3(10, 10, 10);
    else lightPos = sun->getTransform().position;

    for (const auto& entityPtr : entities) {

        Entity* entity = entityPtr.get();
        if (!entity) continue;

        glm::mat4 model = entity->getTransformMatrix();

        Mesh* mesh = assetManager.getMesh(entity->getMeshID());
        if (!mesh) continue;
        for (auto& sm : mesh->subMeshes)
        {
            UUID instID = entity->getMaterialInstanceUUIDs().at(sm.getName());
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


            shader->use();

            // transforms
            shader->setMat4("model", model);
            shader->setMat4("view", camera.getViewMatrix());
            shader->setMat4("projection", camera.getProjectionMatrix());
            shader->setVec3("cameraPos", camera.getCameraPosition());
            shader->setVec3("viewPos", camera.getCameraPosition());

            // lighting
            shader->setVec3("lightColor", lightColor);
            shader->setVec3("lightPos", lightPos);

            // material
            shader->setVec3("material.Ka", finalMat.Ka);
            shader->setVec3("material.Kd", finalMat.Kd);
            shader->setVec3("material.Ks", finalMat.Ks);
            shader->setVec3("material.Ke", finalMat.Ke);
            shader->setFloat("material.Ns", finalMat.Ns);

            // hover highlight
            shader->setBool("isHovered", sm.isHovered);
            
            sm.draw();
            shader->unuse();
        }

    }
}
