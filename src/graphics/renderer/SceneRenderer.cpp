#include "SceneRenderer.h"

namespace Lengine {
  
    void SceneRenderer::init() {
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        assetManager.LoadAllMetaFiles(Paths::Assets);

        gizmoRenderer.initGizmo();
        
    }

    void SceneRenderer::clearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::initScene() {
        activeScene = assetManager.loadScene(Paths::Default_Scenes + "defaultScene.json");
        sceneManager.getScenes().insert(activeScene);
        sceneManager.getScenes().insert(assetManager.loadScene(Paths::GameScenes + "scene1.json"));

        // temporary active scene logic
        sceneManager.setActiveScene(activeScene);
        std::string path = "C:/Users/llakh/OneDrive/Desktop/Projects/LengineGraphics3D/TestGameFolder/assets/Textures/brick.png";
        assetManager.loadTexture(UUID(5416132005131648543), path);

    }
    void SceneRenderer::renderScene() {
         
        gizmoRenderer.drawGizmoGrid();
        renderer.renderScene(*sceneManager.getActiveScene(), camera, assetManager);
      //  gizmoRenderer.drawGizmoSpheres();
      //  gizmoRenderer.drawGizmoArrows();

    }

    void SceneRenderer::endFrame() {

    }
}
