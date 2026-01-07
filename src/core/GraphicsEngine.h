#pragma once



#include <json.hpp>

#include "../core/Lengine.h"
#include "../core/Timer.h"
#include "../core/settings.h"

#include "../graphics/camera/Camera3d.h"
#include "../graphics/renderer/SceneRenderer.h"
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
		void initSettings();
		void run();
	private:
		EngineSettings settings;
		Window window;
		Camera3d camera;
		InputManager inputManager;
		InputHandler inputHandler;
		AssetManager assetManager;
		SceneManager sceneManager;
		SceneRenderer sceneRenderer;
		PostProcessing postProcess;
		GizmoRenderer gizmoRenderer;

		ImGuiLayer imguiLayer;
		EditorLayer* editorLayer = nullptr;
		LogBuffer logBuffer;
		OutputRedirect* redirect = nullptr;

	
		bool isRunning = true;

		
		void initSystems();
		void mainLoop();
		void shutDown();

		
	
	};
}