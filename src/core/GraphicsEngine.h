#pragma once

#include <json.hpp>

#include "../core/Lengine.h"
#include "../core/Timer.h"
#include "../core/settings.h"

#include "../graphics/camera/Camera3d.h"
#include "../graphics/renderer/SceneRenderer.h"

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
		Lengine::Window window;
		Lengine::Camera3d camera;
		Lengine::InputManager inputManager;
		Lengine::InputHandler inputHandler;
		Lengine::AssetManager assetManager;
		Lengine::SceneManager sceneManager;
		Lengine::SceneRenderer sceneRenderer;

		ImGuiLayer imguiLayer;
		EditorLayer* editorLayer = nullptr;
		LogBuffer logBuffer;
		OutputRedirect* redirect = nullptr;

	
		bool isRunning = true;

		
		void initSystems();
		void mainLoop();
	
	};
}