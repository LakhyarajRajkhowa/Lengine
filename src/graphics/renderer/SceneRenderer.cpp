#include "SceneRenderer.h"

#include "../resources/AssetImporter.h"

#include "../utils/objReaderWriter.h"
namespace Lengine {

    void SceneRenderer::Init() {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);      
        glFrontFace(GL_CCW);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        glEnable(GL_MULTISAMPLE);


    }



    void SceneRenderer::initScene() {
        activeScene = sceneManager.getActiveScene();
      
        GLTexture hdrTex = ImageLoader::LoadHDRTexture(Paths::Textures + "Hdr/" + "rogland_clear_night_4k.hdr");
        hdrSkybox.Init(512); // texRes 
        hdrSkybox.SetHDRTexture(hdrTex);

    }

   
  


    void SceneRenderer::RenderScene(const RenderContext& ctx) {

      
        forwardRenderer.Render(
           ctx
        );

       
        // draw skybox as last
        hdrSkybox.Render(camera.getViewMatrix(), camera.getProjectionMatrix());

    }

    void SceneRenderer::endFrame() {

    }
}