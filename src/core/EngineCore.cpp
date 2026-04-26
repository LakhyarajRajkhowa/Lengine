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

        physicsSystem.Init();
    }

    void EngineCore::updateEssentials(const EditorMode& mode)
    {
        Scene* editorScene = sceneManager.GetActiveScene(mode);

        inputManager.Update();
        assetManager.Update(*editorScene);


        transformSystem.Update(editorScene->Transforms(), editorScene->Hierarchys(), editorScene->GetRootEntities());

    }

    void EngineCore::updateRuntime(const EditorMode& mode)
    {
        Scene* editorScene = sceneManager.GetActiveScene(mode);

        UpdateTimer();

        physicsSystem.update(deltaTime, editorScene->Transforms());
        animationSystem.Update(editorScene->Animations(), editorScene->Skeletons(),  deltaTime);
    }


    void EngineCore::pollEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            eventSystem.dispatch(event);
        }
    }

    void EngineCore::run(const EditorMode mode)
    {
        updateEssentials(mode);

        if (mode == EditorMode::PLAY) {
            updateRuntime(mode);
        }

        pollEvents();

    }

    void EngineCore::presentFrame()
    {
        window.swapBuffer();
    }

    void EngineCore::shutdown()
    {
        assetManager.saveAssetDatabase();
        physicsSystem.shutdown();

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

    PhysicsSystem& EngineCore::getPhysicsSystem()
    {
        return physicsSystem;
    }

}