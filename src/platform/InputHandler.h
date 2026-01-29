#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/backends/imgui_impl_sdl2.h>

#include "../graphics/camera/Camera3d.h"
#include "../graphics/geometry/ray.h"
#include "../platform/KeyBindings.h"
#include "../platform/InputManager.h"
#include "../platform/Window.h"
#include "../scene/Scene.h"

#include "../editor/imgui/ImguiLayer.h"
#include "../editor/EditorLayer.h"

namespace Lengine {
	class InputHandler {
	public:
		InputHandler(Camera3d& cam, InputManager& inputmgr, Window& win, bool& running) :
			camera(cam),
			inputManager(inputmgr),
			window(win),
			isRunning(running)

		{
		}

		void handleInputs(ImGuiLayer& imguiLayer, EditorLayer& editorLayer, const float deltaTime);
	
		Camera3d& camera;
		InputManager& inputManager;
		
		Window& window;
		bool& isRunning;


	private:
		

	};
}