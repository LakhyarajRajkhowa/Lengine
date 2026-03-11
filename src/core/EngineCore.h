#pragma once

#include <json.hpp>

#include "../core/Lengine.h"
#include "../core/Timer.h"
#include "../core/settings.h"
#include "../core/EventSystem.h"

#include "../graphics/renderer/RenderPipeline.h"

#include "../platform/InputManager.h"
#include "../platform/Window.h"

namespace Lengine {

    class EngineCore {
    public:

        EngineCore();

        void initSystems();
        void run();
        void presentFrame();
        void shutdown();

        void pollEvents();

        bool& isRunning();

        Window& getWindow();
        InputManager& getInputManager();
        EventSystem& getEventSystem();
        AssetManager& getAssetManager();
        SceneManager& getSceneManager();
        RenderPipeline& getRenderPipeline();
        RenderSettings& getRenderSettings();
        RuntimeStats& getRuntimeStats();

    private:

        void UpdateTimer();
        void updateRuntime();


    private:

        RuntimeStats runtimeStats;
        EngineSettings settings;
        RenderSettings renderSettings;

        Window window;
        InputManager inputManager;
        EventSystem eventSystem;
        AssetManager assetManager;
        SceneManager sceneManager;
        RenderPipeline renderPipeline;

        bool running = true;

        float deltaTime;

    };

}