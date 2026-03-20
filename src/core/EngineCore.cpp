#include "EngineCore.h"

namespace Lengine {

    EngineCore::EngineCore() :

        window(
            settings.windowName,
            settings.windowWidth,
            settings.windowHeight,
            settings.windowMode
        ),

        sceneManager(assetManager),
        assetManager(settings),
        renderPipeline(assetManager),
        animationSystem(assetManager)

    {
    }

    void EngineCore::initSystems()
    {
        InitTimer();

        renderSettings.resolution_X = settings.resolution_X;
        renderSettings.resolution_Y = settings.resolution_Y;
     
        std::vector<std::string> scenesTobeLoaded;
        scenesTobeLoaded.push_back("emptyScene");

        sceneManager.loadScenes(scenesTobeLoaded);

        assetManager.Init();

        renderPipeline.Init();
    }

    void EngineCore::updateRuntime()
    {
        UpdateTimer();

        inputManager.Update();

        assetManager.Update(*sceneManager.getActiveScene());

        sceneManager.getActiveScene()->Update();

        animationSystem.Update(sceneManager.getActiveScene(), deltaTime);
    }

    void EngineCore::pollEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            eventSystem.dispatch(event);
        }
    }

    void EngineCore::run()
    {

        updateRuntime();

        pollEvents();

    }

    void EngineCore::presentFrame()
    {
        window.swapBuffer();
    }

    void EngineCore::shutdown()
    {
        assetManager.saveAssetDatabase();

        window.quitWindow();
    }

    void EngineCore::UpdateTimer()
    {
        runtimeStats.frameStats = LimitFPS(runtimeStats.targetFPS, runtimeStats.limitFPS);
        deltaTime = runtimeStats.frameStats.deltaTime;
    }

    bool& EngineCore::isRunning()
    {
        return running;
    }

    Window& EngineCore::getWindow()
    {
        return window;
    }

    InputManager& EngineCore::getInputManager()
    {
        return inputManager;
    }

    EventSystem& EngineCore::getEventSystem()
    {
        return eventSystem;
    }

    AssetManager& EngineCore::getAssetManager()
    {
        return assetManager;
    }

    SceneManager& EngineCore::getSceneManager()
    {
        return sceneManager;
    }

    RenderPipeline& EngineCore::getRenderPipeline()
    {
        return renderPipeline;
    }

    RenderSettings& EngineCore::getRenderSettings()
    {
        return renderSettings;
    }

    RuntimeStats& EngineCore::getRuntimeStats()
    {
        return runtimeStats;
    }

}