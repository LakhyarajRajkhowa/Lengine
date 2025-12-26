#pragma once
#include <map>
#include <string>

#include "../graphics/opengl/GLTexture.h"
#include "../resources/ImageLoader.h"

namespace Lengine {

    class TextureCache
    {
    private:
        std::map<std::string, GLTexture> _textureMap;
    public:
        TextureCache();
        ~TextureCache();

        GLTexture getTexture(const std::string& texturePath);
        bool attachTexture(
            const std::string& texturePath,
            GLTexture* outTexture
        );
    };
}

