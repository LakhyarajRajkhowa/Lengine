#include <Windows.h>

#define SDL_MAIN_HANDLED


#include "../core/GraphicsEngine.h"

int main(int argc, char* argv[]) {

    Lengine::GraphicsEngine engine;
    engine.run();

    return 0;

}



