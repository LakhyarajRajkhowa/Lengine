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

void GizmoRenderer::initGizmoArrows()
{
    Mesh* arrowMesh = assetManager.getMesh(MeshID::Arrow);

    // X Axis (Red)
    arrowX.gizmoArrow = arrowMesh;
    arrowX.axis = glm::vec3(1, 0, 0);
    arrowX.color = glm::vec4(1, 0, 0, 1);

    // Y Axis (Green)
    arrowY.gizmoArrow = arrowMesh;
    arrowY.axis = glm::vec3(0, 1, 0);
    arrowY.color = glm::vec4(0, 1, 0, 1);

    // Z Axis (Blue)
    arrowZ.gizmoArrow = arrowMesh;
    arrowZ.axis = glm::vec3(0, 0, 1);
    arrowZ.color = glm::vec4(0, 0, 1, 1);

    for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {
        a->gizmoArrowShader.compileShaders(
            Paths::Shaders + "defaultSimpleShader.vert",
            Paths::Shaders + "defaultSimpleShader.frag"
        );
        a->gizmoArrowShader.linkShaders();
    }
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
        // CASE 2: Mesh exists → draw spheres if only singular mesh
        // --------------------------------------------
        if(m->subMeshes.size() == 1)  
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

void GizmoRenderer::drawGizmoArrows()
{
    for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {

        GLSLProgram& shader = a->gizmoArrowShader;
        shader.use();
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());
        
    }



    for (auto& e : sceneManager.getActiveScene()->getEntities()) {
        if (!e->isSelected )
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

            drawSingleArrow(arrowX, pos, scale.x * r);
            drawSingleArrow(arrowY, pos, scale.y * r);
            drawSingleArrow(arrowZ, pos, scale.z * r);
            continue;
        }

        // --------------------------------------------
        // CASE 2: Mesh exists → draw spheres for each submesh
        // --------------------------------------------
        if (m->subMeshes.size() == 1)
            for (auto& sm : m->subMeshes)
            {
                float r = sm.getBoundingRadius();
                glm::vec3 offset = sm.getLocalCenter() * scale;

                glm::vec3 arrowPos = pos + offset;

                drawSingleArrow(arrowX, arrowPos, scale.x * r);
                drawSingleArrow(arrowY, arrowPos, scale.y * r);
                drawSingleArrow(arrowZ, arrowPos, scale.z * r);
            }
        
        for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {

            GLSLProgram& shader = a->gizmoArrowShader;
            shader.unuse();


        }
    }
}



void GizmoRenderer::drawSingleArrow(
    GizmoArrows& arrow,
    const glm::vec3& position,
    float size
) {
    if (!arrow.gizmoArrow)
        return;

    GLSLProgram& shader = arrow.gizmoArrowShader;
    shader.setVec4("color", arrow.color);
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);

    // Axis orientation
    if (arrow.axis == glm::vec3(1, 0, 0))
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
    else if (arrow.axis == glm::vec3(0, 0, 1))
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));

    float length = size * 1.5f;   // arrow length
    float thickness =  1.0f;   // constant breadth

    model = glm::scale(model, glm::vec3(
        thickness,
        length,
        thickness
    ));

    shader.setMat4("model", model);
    arrow.gizmoArrow->draw();
}
