#include "SceneRenderer.h"

namespace Lengine {
  
    void SceneRenderer::init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);      // Remove back faces
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);


    }

    void SceneRenderer::preloadAssets(){
        assetManager.LoadAllMetaFiles(Paths::Assets);
        assetManager.loadAssetRegistry(Paths::GameAssetRegistryFolder + "assetRegistry_hallway_scene.json");
    }

    void SceneRenderer::clearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::initScene() {
        gizmoRenderer.initGizmo();

        activeScene = assetManager.loadScene(Paths::GameScenes + "hallway_scene.json");
        sceneManager.getScenes().insert(activeScene);

        // temporary active scene logic
        sceneManager.setActiveScene(activeScene);
       

    }
    void SceneRenderer::renderScene(EditorConfig& edtitorConfig) {
       
        glDisable(GL_CULL_FACE);
        gizmoRenderer.drawGizmoGrid();
        gizmoRenderer.drawGizmoArrows();
        glEnable(GL_CULL_FACE);

        renderer.renderScene(*sceneManager.getActiveScene(), edtitorConfig);
        
    }

    void SceneRenderer::endFrame() {

    }
}
