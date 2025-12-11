#include "Renderer.h"

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

        for (auto& sm : mesh->subMeshes) {
            Material* material = nullptr;
            GLSLProgram* shader = nullptr;
            
                material = sm.getMaterial();
                shader = material->getShader();
            
           
            if (!material || !shader) continue;
            shader->use();

            // transforms
           
            shader->setMat4("model", model);
            shader->setMat4("view", camera.getViewMatrix());
            shader->setMat4("projection", camera.getProjectionMatrix());
            shader->setVec3("cameraPos", camera.getCameraPosition());
            shader->setVec3("viewPos", camera.getCameraPosition());

            // lighting 
            shader->setVec3("lightColor", lightColor);
            shader->setFloat("ambientStrength", ambientStrength);
            shader->setVec3("lightPos", lightPos);
            shader->setFloat("specularStrength", specularStrength);
            shader->setVec3("objectColor", material->objectColor);
           
            // material 
           // material->apply();

            // draw  
            sm.draw();

            shader->unuse();
        }

        
    }
}
