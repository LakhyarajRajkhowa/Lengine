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
        static GLTexture loadTexture2D(
            const std::string& filePath,
            bool srgb
        );
        static GLTexture loadTextureCubeMap(std::vector<std::string> filePaths);

        static ImageData stbiLoader(const std::string& filePath);
        static void uploadToGPU(const ImageData& img, bool srgb);
       
        
    };
}
