#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
namespace Lengine {
    struct ImageData {
        int width;
        int height;
        int channels;
        std::vector<unsigned char> pixels;
    };


    struct  GLTexture
    {
        GLuint id;
        int width;
        int height;
        int channels = 0;
        ImageData imageCPU;

        std::string name;
        std::string path;
 
        bool pendingGPUUpload = false;

       
    };
}
