#pragma once
#include "../utils/UUID.h"

#include "../core/paths.h"
namespace Lengine {

    struct ShaderAsset
    {
        std::string name;
        std::string vertexShaderPath;
        std::string fragemnetShaderPath;

    };

    struct ShaderRegistry {
        static inline const ShaderAsset universalPbr = ShaderAsset{
            "Universal PBR shader",
            ShaderPath::pbrVertexShaderPath,
            ShaderPath::pbrFragmentShaderPath
        };

        static const std::vector<ShaderAsset>& GetAllDefaults()
        {
            static const std::vector<ShaderAsset> defaults = {
                universalPbr
            };

            return defaults;
        }

    };




}
