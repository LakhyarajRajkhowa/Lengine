#include "SceneRenderer.h"

namespace Lengine {
  
    void SceneRenderer::init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);      // Remove back faces
        glFrontFace(GL_CCW);
        assetManager.LoadAllMetaFiles(Paths::Assets);

        gizmoRenderer.initGizmo();
        
    }

    void SceneRenderer::clearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::initScene() {
        activeScene = assetManager.loadScene(Paths::GameScenes + "defaultScene.json");
        sceneManager.getScenes().insert(activeScene);
        sceneManager.getScenes().insert(assetManager.loadScene(Paths::GameScenes + "scene1.json"));

        // temporary active scene logic
        sceneManager.setActiveScene(activeScene);
       

    }
    void SceneRenderer::renderScene() {
       

        renderer.renderScene(*sceneManager.getActiveScene(), camera, assetManager);
        
        glDisable(GL_CULL_FACE);
        gizmoRenderer.drawGizmoGrid();
         // gizmoRenderer.drawGizmoSpheres();
        //  gizmoRenderer.drawGizmoArrows();
        glEnable(GL_CULL_FACE);
    }

    void SceneRenderer::endFrame() {

    }
}
