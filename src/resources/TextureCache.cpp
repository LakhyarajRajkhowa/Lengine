#include "TextureCache.h"

namespace Lengine {

    TextureCache::TextureCache(/* args */)
    {
    }

    TextureCache::~TextureCache()
    {
    }

    GLTexture TextureCache::getTexture(const std::string& texturePath, bool sRGB) {

        auto mit = _textureMap.find(texturePath);

        if (mit == _textureMap.end()) {
            //GLTexture newTexture = ImageLoader::loadPNG(texturePath);
            GLTexture newTexture = ImageLoader::loadTexture2D(texturePath, sRGB);
            std::pair<std::string, GLTexture> newPair(texturePath, newTexture);

            _textureMap.insert(make_pair(texturePath, newTexture));

            return newTexture;
        }

        return mit->second;
    };

    bool TextureCache::attachTexture(
        const std::string& texturePath,
        GLTexture* outTexture
    )
    {
        auto it = _textureMap.find(texturePath);

        // Already cached
        if (it != _textureMap.end())
        {
            *outTexture = (it->second); // or copy handle if safe
            return true;
        }

        // Load new texture
        *outTexture = ImageLoader::loadTexture2D(texturePath, false);

        // Store pointer
        _textureMap[texturePath] = *outTexture;

        return false;
    }

}
