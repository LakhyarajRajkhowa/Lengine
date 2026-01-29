#include "Gizmos.h"

using namespace Lengine;

void GizmoRenderer::initGizmoGrid() {
    gizmoGrid = assetManager.GetSubmesh(SubmeshID::Plane); 
    gizmoGridShader.compileShaders(Paths::Shaders + "grid.vert", Paths::Shaders + "grid.frag");
    gizmoGridShader.linkShaders();

}
void GizmoRenderer::initGizmoSpheres() {
	gizmoSphere = assetManager.GetSubmesh(SubmeshID::Sphere); // uuid of sphere
	gizmoSphereShader.compileShaders(Paths::Shaders + "boundingSphere.vert", Paths::Shaders + "boundingSphere.frag");
	gizmoSphereShader.linkShaders();

}

void GizmoRenderer::initGizmoArrows()
{
    Submesh* arrowMesh = assetManager.GetSubmesh(SubmeshID::Arrow);

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
    if(gizmoGrid) gizmoGrid->draw();
    gizmoGridShader.unuse();


}


void GizmoRenderer::drawGizmoSpheres() {

    if (EditorSelection::GetEntity() == UUID::Null) return;

    Scene* activeScene = sceneManager.getActiveScene();

    Entity* e = activeScene->getEntityByID(EditorSelection::GetEntity());

    if (!e) return;
    if (!activeScene->Transforms().Has(e->getID())) return;

    TransformComponent& tr = activeScene->Transforms().Get(e->getID());

    gizmoSphereShader.use();
    gizmoSphereShader.setMat4("view", camera.getViewMatrix());
    gizmoSphereShader.setMat4("projection", camera.getProjectionMatrix());
    gizmoSphereShader.setVec4("color", glm::vec4(1, 1, 1, 0.1f));


    glm::vec3 center = tr.GetWorldPosition();
    float radius = 1.0f; // fallback radius

    // -------------------------------
    // Try mesh-based bounds
    // -------------------------------
    if (activeScene->MeshFilters().Has(e->getID()))
    {
        UUID meshID = activeScene->MeshFilters().Get(e->getID()).submeshID;
        if (!meshID.isNull())
        {
            if (Submesh* m = assetManager.GetSubmesh(meshID))
            {
                glm::vec3 scaledCenter = m->getLocalCenter() * tr.localScale;

                float maxScale = glm::max(tr.localScale.x,
                    glm::max(tr.localScale.y, tr.localScale.z));

                center += scaledCenter;
                radius = m->getBoundingRadius() * maxScale;
            }

            if (Submesh* m = assetManager.GetSubmesh(meshID))
            {
                glm::vec3 localCenter = m->getLocalCenter();

                glm::vec3 worldCenter =
                    glm::vec3(tr.worldMatrix * glm::vec4(localCenter, 1.0f));

                glm::vec3 worldScale = tr.GetWorldScale();
                float maxScale = glm::max(worldScale.x, glm::max(worldScale.y, worldScale.z));

                center = worldCenter;
                radius = m->getBoundingRadius() * maxScale;
            }

        }
    }

    // -------------------------------
    // Build model matrix ONCE
    // -------------------------------
    glm::mat4 model(1.0f);
    model = glm::translate(model, center);
    model = glm::scale(model, glm::vec3(radius));

    gizmoSphereShader.setMat4("model", model);
    if (gizmoSphere)
        gizmoSphere->draw();
    

    gizmoSphereShader.unuse();
}


void GizmoRenderer::drawGizmoArrows()
{
    if (EditorSelection::GetEntity() == UUID::Null) return;

    Scene* activeScene = sceneManager.getActiveScene();

    Entity* e = activeScene->getEntityByID(EditorSelection::GetEntity());

    if (!e) return;
    if (!activeScene->Transforms().Has(e->getID())) return;

    // Activate shaders once
    for (GizmoArrows* a : { &arrowX, &arrowY, &arrowZ }) {
        GLSLProgram& shader = a->gizmoArrowShader;
        shader.use();
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());
    }

   
    TransformComponent& tr = activeScene->Transforms().Get(e->getID());
    glm::vec3 scale = tr.GetWorldScale();

    // -------------------------------
    // Decide gizmo source
    // -------------------------------
    glm::vec3 center = tr.GetWorldPosition();
    float baseRadius = 1.0f; // fallback radius

    bool hasValidMesh = false;

    if (activeScene->MeshFilters().Has(e->getID()))
    {
        UUID meshID = activeScene->MeshFilters().Get(e->getID()).submeshID;
        if (!meshID.isNull())
        {
            if (Submesh* m = assetManager.GetSubmesh(meshID))
            {
                hasValidMesh = true;
                center = glm::vec3(
                    tr.worldMatrix * glm::vec4(m->getLocalCenter(), 1.0f)
                );

                baseRadius = m->getBoundingRadius();
            }
        }
    }

    // -------------------------------
    // Compute final gizmo size
    // -------------------------------
    float maxScale = glm::max(scale.x, glm::max(scale.y, scale.z));
    float arrowSize = baseRadius * maxScale;

    float capsuleRadius = glm::min(1.0f, arrowSize);
    float capsuleLength = arrowSize;

    // -------------------------------
    // Draw arrows
    // -------------------------------
    drawSingleArrow(arrowX, center, arrowSize, capX.hovered);
    drawSingleArrow(arrowY, center, arrowSize, capY.hovered);
    drawSingleArrow(arrowZ, center, arrowSize, capZ.hovered);

    // -------------------------------
    // Setup capsules
    // -------------------------------
    capX.start = center;
    capX.radius = capsuleRadius;
    capX.end = center + glm::vec3(1, 0, 0) * capsuleLength;

    capY.start = center;
    capY.radius = capsuleRadius;
    capY.end = center + glm::vec3(0, 1, 0) * capsuleLength;

    capZ.start = center;
    capZ.radius = capsuleRadius;
    capZ.end = center + glm::vec3(0, 0, 1) * capsuleLength;
    

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
    if(arrow.gizmoArrow) arrow.gizmoArrow->draw();
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