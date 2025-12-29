#include "Window.h"
#include <iostream>
namespace Lengine {

    Window::Window()
    {
    }

    Window::~Window()
    {
    }

    int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
    {
        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ;

        if (currentFlags & INVISIBLE) {
            flags |= SDL_WINDOW_HIDDEN;
        }

        if (currentFlags & BORDERLESS) {
            flags |= SDL_WINDOW_BORDERLESS;
        }

       
        SDL_DisplayMode mode;
        SDL_GetCurrentDisplayMode(0, &mode);

        if (screenWidth > mode.w) screenWidth = mode.w;
        if (screenHeight > mode.h) screenHeight = mode.h;

        // MSAA
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);



        _sdlWindow = SDL_CreateWindow(
            windowName.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            screenWidth,
            screenHeight,
            flags
        );

        if (!_sdlWindow)
            fatalError("SDL Window could not be created");

        _glContext = SDL_GL_CreateContext(_sdlWindow);
        if (!_glContext)
            fatalError("OpenGL context could not be created");

        GLenum error = glewInit();
        if (error != GLEW_OK)
            fatalError("Could not initialise glew");

        SDL_GL_SetSwapInterval(0);

        if (currentFlags & FULLSCREEN)
        {
            SDL_SetWindowBordered(_sdlWindow, SDL_TRUE); 
            SDL_SetWindowFullscreen(_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }

        SDL_GL_GetDrawableSize(_sdlWindow, &_screenWidth, &_screenHeight);

        

        return 0;
    }


    void Window::swapBuffer() {
      
        SDL_GL_SwapWindow(_sdlWindow);

    }

    void Window::quitWindow() {
        SDL_DestroyWindow(_sdlWindow);
    }



    
}



