#pragma once

#include <external/json.hpp>

#include "core/Timer.h"
#include "core/settings.h"
#include "core/EventSystem.h"

#include "graphics/renderer/RenderPipeline.h"
#include "platform/InputManager.h"
#include "platform/Window.h"

#include "animations/AnimationSystem.h"
#include "transform/TransformSystem.h"
#include "physics/PhysicsSystem.h"

#include "utils/fps.h"

namespace Lengine {

    enum class EngineMode
    {
        Editor,
        Play
    };

    class EngineCore {
    public:

        EngineCore();

        void initSystems();
        void run(const EditorMode mode);
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

        PhysicsSystem& getPhysicsSystem();

    private:

        void UpdateTimer();
        void updateRuntime(const EditorMode& mode);
        void updateEssentials(const EditorMode& mode);



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

        AnimationSystem animationSystem;
        TransformSystem transformSystem;
        PhysicsSystem physicsSystem;

        bool running = true;

        float deltaTime;

    };

}