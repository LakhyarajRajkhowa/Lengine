#include "SceneRenderer.h"

#include "../resources/AssetImporter.h"

#include "../utils/objReaderWriter.h"
namespace Lengine {

    void SceneRenderer::init() {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);      
        glFrontFace(GL_CCW);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_STENCIL_TEST);

        glEnable(GL_MULTISAMPLE);


        shadowMap.init();
        shadowCubeMap.init();

    }

    void SceneRenderer::preloadAssets() {
        AssetDatabase::LoadDatabase();

        assetManager.LoadAllDefaultAssets();

    }



    void SceneRenderer::initScene() {
        gizmoRenderer.initGizmo();
        activeScene = sceneManager.getActiveScene();
       // assetManager.ImportPrefab("C:/Users/llakh/Downloads/hallway_game_ready/scene.gltf", *activeScene);
       // assetManager.LoadPrefabToScene("C:\\Users\\llakh\\OneDrive\\Desktop\\Projects\\LengineGraphics3D\\TestGameFolder\\Library\\Assets\\Prefab\\broken_brick_wall_4k.gltf\\broken_brick_wall_4k.prefab", *activeScene);
        /*
        skybox.init();
       
        std::vector<std::string> faces
        {
                Paths::Default_skybox + "right.jpg",
                Paths::Default_skybox + "left.jpg",
                Paths::Default_skybox + "top.jpg",
                Paths::Default_skybox + "bottom.jpg",
                Paths::Default_skybox + "front.jpg",
                Paths::Default_skybox + "back.jpg"
        };
        GLTexture cubemapTexture = ImageLoader::loadTextureCubemap(faces);

        skybox.setCubemapTexture(cubemapTexture);
        */
        

        GLTexture hdrTex = ImageLoader::LoadHDRTexture(Paths::Textures + "Hdr/" + "bambanani_sunset_4k.hdr");
        hdrSkybox.Init(1024); // texRes 
        hdrSkybox.SetHDRTexture(hdrTex);

    }

    void SceneRenderer::RenderFrame(const EditorConfig& config)
    {

        
        if (renderSettings.MSAA)
        {
            // MSAA enabled — render to MSAA framebuffer (LDR or HDR)
            if (renderSettings.HDR)
                renderHDR_MSAA(config);   // MSAA + HDR
            else
                renderMSAA_LDR(config);   // MSAA + LDR
        }
        else
        {
            // MSAA disabled
            if (renderSettings.HDR)
                renderHDR(config);        // HDR without MSAA
            else
                renderLDR(config);        // normal LDR
        }
    }

    
    void SceneRenderer::renderLDR(const EditorConfig& config)
    {
        auto& ldr = viewport.GetFramebuffer();

        ldr.Bind();
        viewport.ClearFrame({ 0,0,0,1 });
        RenderScene_phong(config);
        ldr.Unbind();
    }

    void SceneRenderer::renderMSAA_LDR(const EditorConfig& config)
    {
        auto& msaa = viewport.GetMSAAFramebuffer(); // multisample LDR
        auto& ldr = viewport.GetFramebuffer();        // normal LDR to display

        // 1️⃣ Render scene to MSAA framebuffer
        msaa.Bind();
        viewport.ClearFrame({ 0,0,0,1 });
        RenderScene_phong(config); // your usual scene render
        msaa.Unbind();

        // 2️⃣ Resolve MSAA → normal LDR framebuffer
        msaa.ResolveTo(ldr);

    }


    void SceneRenderer::renderHDR(const EditorConfig& config)
    {
        auto& hdr = viewport.GetHDRFramebuffer();
        auto& ldr = viewport.GetFramebuffer();

        // 1. Scene → HDR
        hdr.Bind();
        viewport.ClearFrame({ 0,0,0,1 });
        RenderScene_phong(config);
        hdr.Unbind();

        // 2. Bloom (optional)
        if (renderSettings.enableBloom)
        {
            postProcess.renderBloom(
                hdr.GetColorBuffers()[1],
                renderSettings.bloomBlur 
            );
        }

        // 3. Tone map → LDR
        ldr.Bind();
        ldr.UseTexture(hdr.GetColorBuffers()[0], 0);

        if (renderSettings.enableBloom) {
            ldr.UseTexture(postProcess.getBloomColorBuffer(), 1);
        }

        viewport.ClearFrame({ 0,0,0,1 });

        postProcess.renderToneMapping(renderSettings.enableBloom, renderSettings.exposure);

        ldr.Unbind();


    }

    void SceneRenderer::renderHDR_MSAA(const EditorConfig& config)
    {
        auto& msaaHdr = viewport.GetMSAAHDRFramebuffer();
        auto& hdr = viewport.GetHDRFramebuffer();
        auto& ldr = viewport.GetFramebuffer();

        // 1. Scene → MSAA HDR
        msaaHdr.Bind();
        viewport.ClearFrame({ 0,0,0,1 });
        RenderScene_phong(config);
        msaaHdr.Unbind();

        // 2. Resolve MSAA → HDR
        msaaHdr.ResolveToHDR(hdr);


        // 3. Bloom (optional)
        if (renderSettings.enableBloom)
        {
            postProcess.renderBloom(
                hdr.GetColorBuffers()[1],
                renderSettings.bloomBlur
            );
        }

        // 4. Tone map → LDR
        ldr.Bind();

        ldr.UseTexture(hdr.GetColorBuffers()[0], 0);

        if (renderSettings.enableBloom)
            ldr.UseTexture(postProcess.getBloomColorBuffer(), 1);

      
        viewport.ClearFrame({ 0,0,0,1 });
        
        postProcess.renderToneMapping(renderSettings.enableBloom, renderSettings.exposure);

        ldr.Unbind();

    }
    

    // TODO : Fix Frame stuck at Framebuffer change !!!

    void SceneRenderer::OnRenderSettingsChanged()
    {
        if (!renderSettings.needsReload) return;


        auto& msaaHdr = viewport.GetMSAAHDRFramebuffer();
        auto& msaa = viewport.GetMSAAFramebuffer();
        auto& hdr = viewport.GetHDRFramebuffer();

        msaaHdr.Destroy();
        msaa.Destroy();
        hdr.Destroy();
        


        if (renderSettings.HDR && renderSettings.MSAA)
        {
            msaaHdr.SetMSAASamples(renderSettings.msaaSamples);
            msaaHdr.Create();
            hdr.Create();

        }
        else if (renderSettings.HDR)
        {
            hdr.Create();
        }
        else if (renderSettings.MSAA)
        {
            msaa.SetMSAASamples(renderSettings.msaaSamples);
            msaa.Create();
        }



        renderSettings.needsReload = false;
    }


    void SceneRenderer::renderShadowPass()
    {
        Scene* scene = sceneManager.getActiveScene();
        auto& entities = scene->getEntities();
        auto& mrs = scene->MeshRenderers();

        // TODO : Peter panning 

       // shadowMap.renderDepthMap(entities, mrs, directionalLight, assetManager);
        //shadowCubeMap.renderDepthCubeMap(entities, mrs, pointLight, assetManager);

        glViewport(
            0, 0,
            settings.resolution_X,
            settings.resolution_Y
        );

    }


    void SceneRenderer::UpdateScene() {
        activeScene = sceneManager.getActiveScene();
        activeScene->Update();
    }

    void SceneRenderer::RenderScene_phong(const EditorConfig& edtitorConfig) {

      
        glDisable(GL_CULL_FACE);
        gizmoRenderer.drawGizmoGrid();
        gizmoRenderer.drawGizmoArrows();
       // gizmoRenderer.drawGizmoSpheres();
        glEnable(GL_CULL_FACE);


        forwardRenderer.Render(
            *activeScene,
            edtitorConfig,
            shadowMap,
            shadowCubeMap,
            hdrSkybox.GetIrradianceMap()
        );


        // draw skybox as last
        hdrSkybox.Render(camera.getViewMatrix(), camera.getProjectionMatrix());

    }

    void SceneRenderer::endFrame() {

    }
}