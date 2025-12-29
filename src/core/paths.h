#pragma once
#include <string>
#include <filesystem>

#include "../core/settings.h"
namespace Lengine {

    struct Paths
    {
        
        static inline std::string Assets = "../assets/";
        static inline std::string Shaders = Assets + "shaders/";
        static inline std::string Materials = Assets + "Materials/";
        static inline std::string Textures = Assets + "Textures/";
        static inline std::string Mesh = Assets + "Mesh/";
        static inline std::string Icons = Assets + "icons/";
        static inline std::string Defaults = "../defaults/";
        static inline std::string Default_Scenes = Defaults + "scenes/";
        static inline std::string Default_skybox = Defaults + "skybox/";

        static inline std::string ActiveGameFolder;

        static inline std::string GameAssets;
        static inline std::string GameAssets_Shaders;
        static inline std::string GameAssets_Materials;
        static inline std::string GameAssets_Mesh;
        static inline std::string GameAssets_Textures;
        static inline std::string GameScenes;

        static inline std::string GameAssetRegistryFolder;

        static void setGameFolderPath(const std::string& gameFolderPath)
        {
            ActiveGameFolder = gameFolderPath;

            GameAssets = ActiveGameFolder + "/assets/";
            GameAssets_Shaders = GameAssets + "Shaders/";
            GameAssets_Materials = GameAssets + "Materials/";
            GameAssets_Mesh = GameAssets + "Mesh/";
            GameAssets_Textures = GameAssets + "Textures/";
            GameScenes = ActiveGameFolder + "/scenes/";

            GameAssetRegistryFolder = GameAssets + "AssetRegistry/";
        }

         

    };

  
    struct ShaderPath {
        static std::string Vert(const std::string& name) {
            return Paths::Shaders + name + ".vert";
        }
        static std::string Frag(const std::string& name) {
            return Paths::Shaders + name + ".frag";
        }

        static inline std::string defaultVertexShaderPath = Vert("defaultShader");
        static inline std::string defaultFragmentShaderPath = Frag("defaultShader");

        static inline std::string lightSourceVertexShaderPath = Vert("lightSource");
        static inline std::string lightSourceFragmentShaderPath = Frag("lightSource");
    };

    


}
