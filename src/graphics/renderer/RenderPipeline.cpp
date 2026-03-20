#include "RenderPipeline.h"

using namespace Lengine;

void RenderPipeline::Init() {
    // Depth
    glEnable(GL_DEPTH_TEST);

    // Face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // MSAA
    glEnable(GL_MULTISAMPLE);

    GLTexture hdrTex = ImageLoader::LoadHDRTexture(Paths::Textures + "Hdr/" + "rogland_clear_night_4k.hdr");
    hdrSkybox.Init(512);
    hdrSkybox.SetHDRTexture(hdrTex);

    postProcess.InitHDRShaders();


}

void RenderPipeline::RecreateFramebuffers()
{
    // Destroy old ones automatically by resetting unique_ptr
    mainFramebuffer.reset();
    hdrFramebuffer.reset();
    msaaFramebuffer.reset();


    FramebufferSpecification mainBufferSpec;
    mainBufferSpec.width = renderSettings.resolution_X;
    mainBufferSpec.height = renderSettings.resolution_Y;
    mainBufferSpec.samples = 1;
    mainBufferSpec.colorFormats = { GL_RGBA8 };
    mainBufferSpec.colorAttachmentCount = 1;

    mainFramebuffer = std::make_unique<Framebuffer>(mainBufferSpec);


    FramebufferSpecification hdrSpec;
    hdrSpec.width = renderSettings.resolution_X;
    hdrSpec.height = renderSettings.resolution_Y;
    hdrSpec.samples = 1;
    hdrSpec.colorFormats = { GL_RGBA16F , GL_RGBA16F };
    hdrSpec.colorAttachmentCount = 2; // scene + bright

    hdrFramebuffer = std::make_unique<Framebuffer>(hdrSpec);
    

    // ---------------------------
    // 3 MSAA (optional)
    // ---------------------------

    if (renderSettings.MSAA)
    {
        FramebufferSpecification msaaSpec;
        msaaSpec.width = renderSettings.resolution_X;
        msaaSpec.height = renderSettings.resolution_Y;
        msaaSpec.samples = renderSettings.msaaSamples;

        msaaSpec.colorFormats = { GL_RGBA16F };

        msaaSpec.colorAttachmentCount = 1;

        msaaFramebuffer = std::make_unique<Framebuffer>(msaaSpec);
    }

   
}

void RenderPipeline::PostProcess() {

}

void RenderPipeline::SetRenderSettings(const RenderSettings& settings)
{
    renderSettings = settings;

    RecreateFramebuffers();
    BuildGraph();
}


void RenderPipeline::BuildGraph()
{
    renderGraph.Clear();

    // ---- SHADOW ----
    renderGraph.AddPass(
        std::make_unique<ShadowPass>(
            shadowCubemap,
            shadowMap,
            assetManager
        )
    );

    // ---- FORWARD PASS ----- 

    if (renderSettings.MSAA)
    {
       
        renderGraph.AddPass(
            std::make_unique<ForwardPass>(
                forwardRenderer,
                *msaaFramebuffer
            )
        );
       

        // SKYBOX
        renderGraph.AddPass(
            std::make_unique<SkyboxPass>(
                *msaaFramebuffer,
                hdrSkybox,
                Paths::Textures + "Hdr/" + "wooden_studio_09_4k.hdr",
                512
            )
        );

        

        Framebuffer& resolveTarget =
             *hdrFramebuffer;

        renderGraph.AddPass(
            std::make_unique<ResolvePass>(
                *msaaFramebuffer,
                resolveTarget
            )
        );
       
    }
    else
    {
        Framebuffer& target =
            *hdrFramebuffer;


        renderGraph.AddPass(
            std::make_unique<ForwardPass>(
                forwardRenderer,
                target
            )
        );
 

        // SKYBOX
        renderGraph.AddPass(
            std::make_unique<SkyboxPass>(
                target,
                hdrSkybox,
                Paths::Textures + "Hdr/" + "wooden_studio_09_4k.hdr",
                512
            )
        );
    }


    // ---- POST PROCESS -----

    
    if (renderSettings.enableBloom) {
        renderGraph.AddPass(
            std::make_unique<BloomPass>(
                postProcess,
                *hdrFramebuffer,
                renderSettings.resolution_X,
                renderSettings.resolution_Y

            )
        );
    }



    renderGraph.AddPass(
        std::make_unique<ToneMapPass>(
            postProcess,
            *mainFramebuffer,
            *hdrFramebuffer
        )
    );


  
} 



void RenderPipeline::Render(RenderContext& ctx)
{
    if (ctx.settings->needsReload) {

        SetRenderSettings(*ctx.settings);
        ctx.settings->needsReload = false;



    }
    // 1 Update per-frame context data
    ctx.irradianceMap = hdrSkybox.GetIrradianceMap();
    ctx.shadowMap = &shadowMap;
    ctx.shadowCubeMap = &shadowCubemap;
    ctx.prefilterMap = hdrSkybox.GetPrefilterMap();
    ctx.brdfLUTMap = hdrSkybox.GetbrdfLUTMap();

    // Execute passes in order
    renderGraph.Execute(ctx);
                
}

Framebuffer& RenderPipeline::GetFinalFramebuffer()
{
    return *mainFramebuffer;
}


