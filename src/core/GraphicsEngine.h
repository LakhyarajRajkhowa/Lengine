#pragma once



#include <json.hpp>

#include "../core/Lengine.h"
#include "../core/Timer.h"
#include "../core/settings.h"

#include "../graphics/camera/Camera3d.h"
#include "../graphics/renderer/RenderPipeline.h"
#include "../graphics/renderer/PostProcess/PostProcessing.h"

#include "../platform/InputManager.h"
#include "../platform/InputHandler.h"
#include "../platform/Window.h"

#include "../editor/imgui/ImguiLayer.h"
#include "../editor/EditorLayer.h"


namespace Lengine {
	class GraphicsEngine {
	public:
		GraphicsEngine();
		void run();
	private:
		RuntimeStats runtimeStats;

		EngineSettings settings;
		RenderSettings renderSettings;
		Window window;
		Camera3d camera;
		InputManager inputManager;
		InputHandler inputHandler;
		AssetManager assetManager;
		SceneManager sceneManager;
		GizmoRenderer gizmoRenderer;
		RenderPipeline renderPipeline;
		

		ImGuiLayer imguiLayer;
		EditorLayer editorLayer;
		LogBuffer logBuffer;
		OutputRedirect* redirect = nullptr;

	
		bool isRunning = true;

		
		void initSystems();
		void mainLoop();
		void shutDown();
		void UpdateTimer();

	private:

		float deltaTime;
	
	};
}