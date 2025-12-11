#include "Gizmos.h"

using namespace Lengine;

void GizmoRenderer::initGizmoGrid() {
    gizmoGrid = assetManager.getMesh(UUID(16567679459741108534)); // uuid of plane
    gizmoGridShader.compileShaders("../assets/Shaders/grid.vert", "../assets/Shaders/grid.frag");
    gizmoGridShader.linkShaders();

}
void GizmoRenderer::initGizmoSpheres() {

	gizmoSphere = assetManager.getMesh(UUID(9343755805681254094)); // uuid of sphere
	gizmoSphereShader.compileShaders("../assets/Shaders/boundingSphere.vert", "../assets/Shaders/boundingSphere.frag");
	gizmoSphereShader.linkShaders();

}

void GizmoRenderer::drawGizmoGrid(){
    gizmoGridShader.use();
    gizmoGridShader.setMat4("view", camera.getViewMatrix());
    gizmoGridShader.setMat4("projection", camera.getProjectionMatrix());
    gizmoGridShader.setVec4("color", glm::vec4(0, 1, 0, 0.2));
    glm::mat4 model(1.0f);
    gizmoGridShader.setMat4("model", model);
    gizmoGrid->draw();
    gizmoGridShader.unuse();


}


void GizmoRenderer::drawGizmoSpheres() {
    gizmoSphereShader.use();
    gizmoSphereShader.setMat4("view", camera.getViewMatrix());
    gizmoSphereShader.setMat4("projection", camera.getProjectionMatrix());
    gizmoSphereShader.setVec4("color", glm::vec4(1, 1, 1, 0.1));

    for (auto& e : sceneManager.getActiveScene()->getEntities()) {
        // only selected entity will show gizmo sphere
        if(e->isSelected && e->getMeshID())
            for (auto& sm : assetManager.getMesh(e->getMeshID())->subMeshes) {
                float r = sm.getBoundingRadius();
                glm::vec3 pos = e->getTransform().position;

                glm::mat4 model(1.0f);
                glm::vec3 scaledCenter = sm.getLocalCenter() * e->getTransform().scale;

                model = glm::translate(model, pos + scaledCenter);
                model = glm::scale(model, glm::vec3(r) * e->getTransform().scale);


                gizmoSphereShader.setMat4("model", model);

                // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                
                gizmoSphere->draw();
        }
        // entity created but not given any mesh
        if (e->isSelected && !e->getMeshID())
             {
                float r = 1.0f;
                glm::vec3 pos = e->getTransform().position;

                glm::mat4 model(1.0f);

                model = glm::translate(model, pos);
                model = glm::scale(model, glm::vec3(r) * e->getTransform().scale);


                gizmoSphereShader.setMat4("model", model);


                gizmoSphere->draw();
            }

       
    }

    gizmoSphereShader.unuse();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


