#include "SceneRenderer.h"

namespace Lengine {

    void SceneRenderer::init() {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);      // Remove back faces
        glFrontFace(GL_CCW);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_STENCIL_TEST);

        glEnable(GL_MULTISAMPLE);
        

        shadowMap.init();
        shadowCubeMap.init();

      
    }

    void SceneRenderer::preloadAssets() {
        assetManager.LoadAllMetaFiles(Paths::Assets);
        assetManager.loadAssetRegistry(Paths::GameAssetRegistryFolder + "assetRegistry_defaultScene.json");
    }

    void SceneRenderer::clearFrame(const glm::vec4& clearColor) {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SceneRenderer::initScene() {
        gizmoRenderer.initGizmo();

        activeScene = assetManager.loadScene(Paths::GameScenes + "defaultScene.json");
        sceneManager.getScenes().insert(activeScene);

        // temporary active scene logic
        sceneManager.setActiveScene(activeScene);


    }

    void SceneRenderer::renderShadowPass()
    {
        Scene* scene = sceneManager.getActiveScene();
        auto& entities = scene->getEntities();

        auto& directionalLight = scene->getMainDirectionalLight();
        auto& pointLight = scene->getMainPointLight();

        // Peter panning 
        //  glCullFace(GL_FRONT);
        shadowMap.renderDepthMap(entities, directionalLight, assetManager);
       // glCullFace(GL_BACK);

        shadowCubeMap.renderDepthCubeMap(entities, pointLight, assetManager);

        glViewport(
            0, 0,
            settings.resolution_X,
            settings.resolution_Y
        );

    }

   

   

   

    void SceneRenderer::renderScene(EditorConfig& edtitorConfig) {

        glDisable(GL_CULL_FACE);
        gizmoRenderer.drawGizmoGrid();
        gizmoRenderer.drawGizmoArrows();
        glEnable(GL_CULL_FACE);

        renderer.renderScene(
            *sceneManager.getActiveScene(),
            edtitorConfig,
            shadowMap,
            shadowCubeMap
        );

    }

    void SceneRenderer::endFrame() {

    }
}