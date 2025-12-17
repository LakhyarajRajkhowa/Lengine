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
			camera(cam), inputManager(inputmgr), window(win), isRunning(running)
		{
			fixCam = false;
			moveMode = false;
		}

		void handleInputs(ImGuiLayer& imguiLayer, EditorLayer& editorLayer);
	
		Camera3d& camera;
		InputManager& inputManager;
		
		Window& window;
		bool& isRunning;
		bool fixCam;
		bool moveMode;
		bool confirmSelectedEntity = false;

		bool mouseLeftDown = false;
		bool mouseLeftReleased = false;

		Entity* confirmedSelectedEntity = nullptr;

		SubMesh* selectedSubMesh = nullptr;
		SubMesh* confirmedSelectedSubMesh = nullptr;

		glm::vec3 dragPlaneNormal = glm::vec3(0, 1, 0);  // XZ plane (Y up)
		float dragPlaneHeight;
		glm::vec3 dragStartOffset;

		float dragPlaneY = 0.0f;
		glm::vec3 dragStartPoint;
		glm::vec3 dragOffset;

		Entity* selectedEntity = nullptr;
		int lastMouseX = 0;
		int lastMouseY = 0;



	private:
		


	};
}