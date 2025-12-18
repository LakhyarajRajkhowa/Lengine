#pragma once
#include <GL/glew.h>
#include <string>
namespace Lengine {

    struct  GLTexture
    {
        GLuint id;
        int width;
        int height;

        std::string name;
    };
}
