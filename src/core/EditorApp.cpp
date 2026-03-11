#include "EditorApp.h"

namespace Lengine {

    Editor::Editor(
        Window& window,
        InputManager& inputManager,
        EventSystem& eventSystem,
        AssetManager& assetManager,
        SceneManager& sceneManager,
        RenderSettings& renderSettings,
        RuntimeStats& runtimeStats,
        RenderPipeline& renderPipeline,
        bool& isRunning
    )
        :

        window(window),
        inputManager(inputManager),
        eventSystem(eventSystem),
        assetManager(assetManager),
        sceneManager(sceneManager),
        renderSettings(renderSettings),
        runtimeStats(runtimeStats),
        renderPipeline(renderPipeline),
        isRunning(isRunning),

        imguiLayer(
            inputManager,
            isRunning,
            window.getWindow(),
            window.getGlContext()
        ),
        editorOverlays(assetManager),
        editorLayer(
            logBuffer,
            sceneManager,
            editorOverlays.getGizmos(),
            camera,
            inputManager,
            assetManager,
            renderSettings,
            runtimeStats
        ),

        inputHandler(camera, inputManager, window, isRunning)

    {
    }

    void Editor::init()
    {
        camera.init(
            &inputManager
        );
        redirect = new OutputRedirect(logBuffer);

        eventSystem.addListener(
            [this](const SDL_Event& e)
            {
                imguiLayer.processEvent(e);
            }
        );

        editorOverlays.InitGizmos();
       
    }

    void Editor::run()
    {
        inputHandler.handleInputs(
            imguiLayer,
            editorLayer,
            runtimeStats.frameStats.deltaTime
        );

        imguiLayer.beginFrame();

        RenderContext ctx;
        ctx.scene = sceneManager.getActiveScene();
        ctx.settings = &renderSettings;
        ctx.camera = &camera;


        renderPipeline.Render(ctx);

        editorOverlays.RenderGizmoGrid(ctx, renderPipeline.GetFinalFramebuffer());

        editorLayer.OnImGuiRender(renderPipeline.GetFinalImage());

        assetManager.drawLoadingScreens();
        assetManager.drawImportingScreens();

        imguiLayer.endFrame();
    }

    void Editor::shutdown()
    {
        imguiLayer.shutdown();

        if (redirect)
            delete redirect;
    }

}