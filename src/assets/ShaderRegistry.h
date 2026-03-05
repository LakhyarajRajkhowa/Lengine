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

        static inline const ShaderAsset debug = ShaderAsset{
           "Debug shader",
           ShaderPath::debugVertexShaderPath,
           ShaderPath::debugFragmentShaderPath
        };

        static inline const ShaderAsset outline = ShaderAsset{
           "Outline shader",
           ShaderPath::outlineShaderVertexShaderPath,
           ShaderPath::outlineShaderFragmentShaderPath
        };

        static inline const ShaderAsset gizmoGrid = ShaderAsset{
           "Gizmo Grid",
           ShaderPath::gizmoGridShaderVertexShaderPath,
           ShaderPath::gizmoGridShaderFragmentShaderPath
        };

        static const std::vector<ShaderAsset>& GetAllDefaults()
        {
            static const std::vector<ShaderAsset> defaults = {
                universalPbr,
                debug,
                outline,
                gizmoGrid
            };

            return defaults;
        }

    };




}
