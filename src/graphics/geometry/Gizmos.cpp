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

    Scene* activeScene = sceneManager.getActiveScene();
    for (auto& e : activeScene->getEntities()) {
        if (!e->isSelected)
            continue;

        UUID meshID = activeScene->MeshFilters().Get(e->getID()).meshID;
        Mesh* m = meshID ? assetManager.getMesh(meshID) : nullptr;

        glm::vec3 pos = e->getTransform().position;
        glm::vec3 scale = e->getTransform().scale;

        // --------------------------------------------
        // CASE 1: No mesh
        // --------------------------------------------
        if (!m) {
            float r = 1.0f;

            glm::mat4 model(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(r) * scale);

            gizmoSphereShader.setMat4("model", model);
            gizmoSphere->draw();
            continue;
        }

        // --------------------------------------------
        // CASE 2: Mesh-level bounding sphere
        // --------------------------------------------
        float r = m->getBoundingRadius();
        glm::vec3 localCenter = m->getLocalCenter();

        // Scale center (local → world)
        glm::vec3 scaledCenter = localCenter * scale;

        // IMPORTANT: radius must use max scale component
        float maxScale = glm::max(scale.x, glm::max(scale.y, scale.z));
        float worldRadius = r * maxScale;

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos + scaledCenter);
        model = glm::scale(model, glm::vec3(worldRadius));

        gizmoSphereShader.setMat4("model", model);
        gizmoSphere->draw();
    }

    gizmoSphereShader.unuse();
}


void GizmoRenderer::drawGizmoArrows()
{
    // Activate shaders once
    for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {
        GLSLProgram& shader = a->gizmoArrowShader;
        shader.use();
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());
    }

    Scene* activeScene = sceneManager.getActiveScene();
    for (auto& e : activeScene->getEntities()) {
        if (!e->isSelected)
            continue;

        UUID meshID = activeScene->MeshFilters().Get(e->getID()).meshID;

        glm::vec3 pos = e->getTransform().position;
        glm::vec3 scale = e->getTransform().scale;

        Mesh* m = meshID ? assetManager.getMesh(meshID) : nullptr;

        // --------------------------------------------
        // CASE 1: No mesh or meshID
        // --------------------------------------------
        if (!m) {
            float r = 1.0f;
            float maxScale = glm::max(scale.x, glm::max(scale.y, scale.z));
            float arrowSize = r * maxScale;

            drawSingleArrow(arrowX, pos, arrowSize, capX.hovered);
            drawSingleArrow(arrowY, pos, arrowSize, capY.hovered);
            drawSingleArrow(arrowZ, pos, arrowSize, capZ.hovered);

            float capsuleRadius = (arrowSize > 1.0f) ? 1.0f : arrowSize;
            float capsuleLength = arrowSize;

            capX.start = pos;
            capX.radius = capsuleRadius;
            capX.end = pos + glm::vec3(1, 0, 0) * capsuleLength;

            capY.start = pos;
            capY.radius = capsuleRadius;
            capY.end = pos + glm::vec3(0, 1, 0) * capsuleLength;

            capZ.start = pos;
            capZ.radius = capsuleRadius;
            capZ.end = pos + glm::vec3(0, 0, 1) * capsuleLength;

            continue;
        }


        // --------------------------------------------
        // CASE 2: Mesh-level gizmo
        // --------------------------------------------
        glm::vec3 localCenter =  m->getLocalCenter();
        float localRadius = m->getBoundingRadius();

        glm::vec3 worldCenter = pos + localCenter * scale;

        float maxScale = glm::max(scale.x, glm::max(scale.y, scale.z));
        float arrowSize = localRadius * maxScale;

        drawSingleArrow(arrowX, worldCenter, arrowSize, capX.hovered);
        drawSingleArrow(arrowY, worldCenter, arrowSize, capY.hovered);
        drawSingleArrow(arrowZ, worldCenter, arrowSize, capZ.hovered);


        float capsuleRadius = (arrowSize > 1.0f) ? 1.0f : arrowSize;     // tweak visually
        float capsuleLength = (arrowSize > 1.0f) ? 1.0f : arrowSize;      // arrow body length

        capX.start = worldCenter;
        capX.radius = capsuleRadius;
        capX.end = worldCenter + glm::vec3(1, 0, 0) * capsuleLength;
        
        capY.start = worldCenter;
        capY.radius = capsuleRadius;
        capY.end = worldCenter + glm::vec3(0, 1, 0) * capsuleLength;

        capZ.start = worldCenter;
        capZ.radius = capsuleRadius;
        capZ.end = worldCenter + glm::vec3(0, 0, 1) * capsuleLength;
 

    }

    // Deactivate shaders once
    for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {
        a->gizmoArrowShader.unuse();
    }
}




void GizmoRenderer::drawSingleArrow(
    GizmoArrows& arrow,
    const glm::vec3& position,
    const float& size,
    const bool& highlight
) {
    if (!arrow.gizmoArrow)
        return;

    GLSLProgram& shader = arrow.gizmoArrowShader;
    shader.setVec4("color", arrow.color);
    shader.setBool("isHovered", highlight);
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);

    // Axis orientation
    if (arrow.axis == glm::vec3(1, 0, 0))
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
    else if (arrow.axis == glm::vec3(0, 0, 1))
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));

    float length = ( size > 1.0f ) ? 1.0f: size ;   // arrow length
    float thickness =  ( size > 1.0f ) ? 1.0f: size;   // constant breadth

    model = glm::scale(model, glm::vec3(
        thickness,
        length,
        thickness
    ));

    shader.setMat4("model", model);
    arrow.gizmoArrow->draw();
}


void GizmoRenderer::drawDebugCapsuleForArrow(
    const glm::vec3& position,
    const glm::vec3& axis,
    float size
) {
    if (!gizmoSphere)
        return;

    float length = size ;
    float radius = 0.20f; // same as arrow thickness

    glm::vec3 dir = glm::normalize(axis);

    glm::vec3 start = position;
    glm::vec3 end = position + dir * length;
    glm::vec3 mid = (start + end) * 0.5f;

    gizmoSphereShader.setMat4("view", camera.getViewMatrix());
    gizmoSphereShader.setMat4("projection", camera.getProjectionMatrix());
    gizmoSphereShader.setVec4("color", glm::vec4(1, 1, 1, 0.15f));

    // ----------------------------
    // Start sphere
    // ----------------------------
    glm::mat4 model(1.0f);
    model = glm::translate(model, start);
    model = glm::scale(model, glm::vec3(radius));
    gizmoSphereShader.setMat4("model", model);
    gizmoSphere->draw();

    // ----------------------------
    // End sphere
    // ----------------------------
    model = glm::mat4(1.0f);
    model = glm::translate(model, end);
    model = glm::scale(model, glm::vec3(radius));
    gizmoSphereShader.setMat4("model", model);
    gizmoSphere->draw();

    // ----------------------------
    // Middle stretched sphere (fake cylinder)
    // ----------------------------
    model = glm::mat4(1.0f);
    model = glm::translate(model, mid);

    if (axis == glm::vec3(1, 0, 0)) {
        model = glm::scale(model, glm::vec3(length , radius, radius));
    }
    else if (axis == glm::vec3(0, 1, 0)) {
        model = glm::scale(model, glm::vec3(radius, length , radius));
    }
    else if (axis == glm::vec3(0, 0, 1)) {
        model = glm::scale(model, glm::vec3(radius, radius, length ));
    }

    gizmoSphereShader.setMat4("model", model);
    gizmoSphere->draw();

}

 AxisCapsule& GizmoRenderer::getCapsule(const GizmoAxis axis) {
    if (axis == GizmoAxis::X)
        return capX;
    if (axis == GizmoAxis::Y)
        return capY;
    if (axis == GizmoAxis::Z)
        return capZ;
}