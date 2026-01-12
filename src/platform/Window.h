#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>

#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "../core/Errors.h"

namespace Lengine {

	enum  windowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

	class Window
	{
	public:
		Window(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
		~Window();

		int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);

		void swapBuffer();
		void quitWindow();

		int getScreenWidth() { return _screenWidth; }
		int getScreenHeight() { return _screenHeight; }

		SDL_Window* getWindow() { return _sdlWindow; }
		SDL_GLContext getGlContext() { return _glContext;  }

	private:
		
		SDL_Window* _sdlWindow;
		SDL_GLContext _glContext = nullptr;
		int _screenWidth, _screenHeight;

	};
}



