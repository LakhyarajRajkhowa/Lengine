#pragma once
#include <string>

#include "../graphics/opengl/GLTexture.h"
#include "../external/picopng.h"
#include "../platform/IOManager.h"
#include "../core/Errors.h"
namespace Lengine {


    class ImageLoader
    {
    private:

    public:
        static GLTexture loadPNG(const std::string& filePath);
    };
}
