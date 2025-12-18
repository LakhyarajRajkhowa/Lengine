#include "TextureCache.h"

namespace Lengine {

    TextureCache::TextureCache(/* args */)
    {
    }

    TextureCache::~TextureCache()
    {
    }

    GLTexture TextureCache::getTexture(const std::string& texturePath) {

        auto mit = _textureMap.find(texturePath);

        if (mit == _textureMap.end()) {
            GLTexture newTexture = ImageLoader::loadPNG(texturePath);
            
            std::pair<std::string, GLTexture> newPair(texturePath, newTexture);

            _textureMap.insert(make_pair(texturePath, newTexture));

            return newTexture;
        }

        return mit->second;
    };
}
