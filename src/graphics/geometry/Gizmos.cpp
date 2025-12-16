#include "Gizmos.h"

using namespace Lengine;

void GizmoRenderer::initGizmoGrid() {
    gizmoGrid = assetManager.getMesh(MeshID::Plane); 
    gizmoGridShader.compileShaders(Paths::Shaders + "grid.vert", Paths::Shaders + "grid.frag");
    gizmoGridShader.linkShaders();

}
void GizmoRenderer::initGizmoSpheres() {

	gizmoSphere = assetManager.getMesh(MeshID::Sphere); // uuid of sphere
	gizmoSphereShader.compileShaders(Paths::Shaders + "boundingSphere.vert", Paths::Shaders + "boundingSphere.frag");
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
    gizmoSphereShader.setVec4("color", glm::vec4(1, 1, 1, 0.1f));

    for (auto& e : sceneManager.getActiveScene()->getEntities()) {
        if (!e->isSelected)
            continue;

        UUID meshID = e->getMeshID();
        Mesh* m = meshID ? assetManager.getMesh(meshID) : nullptr;

        glm::vec3 pos = e->getTransform().position;
        glm::vec3 scale = e->getTransform().scale;

        // --------------------------------------------
        // CASE 1: No mesh OR mesh not yet loaded
        // --------------------------------------------
        if (m == nullptr)
        {
            float r = 1.0f;   // default gizmo size

            glm::mat4 model(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(r) * scale);

            gizmoSphereShader.setMat4("model", model);
            gizmoSphere->draw();
            continue;
        }

        // --------------------------------------------
        // CASE 2: Mesh exists → draw spheres for each submesh
        // --------------------------------------------
        for (auto& sm : m->subMeshes)
        {
            float r = sm.getBoundingRadius();
            glm::vec3 scaledCenter = sm.getLocalCenter() * scale;

            glm::mat4 model(1.0f);
            model = glm::translate(model, pos + scaledCenter);
            model = glm::scale(model, glm::vec3(r) * scale);

            gizmoSphereShader.setMat4("model", model);
            gizmoSphere->draw();
        }
    }

    gizmoSphereShader.unuse();
}


