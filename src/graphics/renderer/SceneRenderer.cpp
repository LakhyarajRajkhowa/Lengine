#include "SceneRenderer.h"

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
        assetManager.LoadAllMetaFiles(Paths::Assets);
       // assetManager.loadAssetRegistry(Paths::GameAssetRegistryFolder + "assetRegistry_defaultScene.json");

    }



    void SceneRenderer::initScene() {
        gizmoRenderer.initGizmo();

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
        viewport.clearFrame({ 0,0,0,1 });
        renderScene(config);
        ldr.Unbind();
    }

    void SceneRenderer::renderMSAA_LDR(const EditorConfig& config)
    {
        auto& msaa = viewport.GetMSAAFramebuffer(); // multisample LDR
        auto& ldr = viewport.GetFramebuffer();        // normal LDR to display

        // 1️⃣ Render scene to MSAA framebuffer
        msaa.Bind();
        viewport.clearFrame({ 0,0,0,1 });
        renderScene(config); // your usual scene render
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
        viewport.clearFrame({ 0,0,0,1 });
        renderScene(config);
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
        ldr.useTexture(hdr.GetColorBuffers()[0], 0);

        if (renderSettings.enableBloom) {
            ldr.useTexture(postProcess.getBloomColorBuffer(), 1);
        }

        viewport.clearFrame({ 0,0,0,1 });

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
        viewport.clearFrame({ 0,0,0,1 });
        renderScene(config);
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

        ldr.useTexture(hdr.GetColorBuffers()[0], 0);

        if (renderSettings.enableBloom)
            ldr.useTexture(postProcess.getBloomColorBuffer(), 1);

        viewport.clearFrame({ 0,0,0,1 });

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
            msaaHdr.setMSAASamples(renderSettings.msaaSamples);
            msaaHdr.Create();
            hdr.Create();

        }
        else if (renderSettings.HDR)
        {
            hdr.Create();
        }
        else if (renderSettings.MSAA)
        {
            msaa.setMSAASamples(renderSettings.msaaSamples);
            msaa.Create();
        }



        renderSettings.needsReload = false;
    }


    void SceneRenderer::renderShadowPass()
    {
        Scene* scene = sceneManager.getActiveScene();
        auto& entities = scene->getEntities();

        auto& directionalLight = scene->getMainDirectionalLight();
        auto& pointLight = scene->getMainPointLight();

        // TODO : Peter panning 

        shadowMap.renderDepthMap(entities, directionalLight, assetManager);
        shadowCubeMap.renderDepthCubeMap(entities, pointLight, assetManager);

        glViewport(
            0, 0,
            settings.resolution_X,
            settings.resolution_Y
        );

    }



    void SceneRenderer::renderScene(const EditorConfig& edtitorConfig) {

      
        glDisable(GL_CULL_FACE);
        gizmoRenderer.drawGizmoGrid();
        gizmoRenderer.drawGizmoArrows();
        glEnable(GL_CULL_FACE);

        forwardRenderer.render(
            *sceneManager.getActiveScene(),
            edtitorConfig,
            shadowMap,
            shadowCubeMap
        );

    }

    void SceneRenderer::endFrame() {

    }
}