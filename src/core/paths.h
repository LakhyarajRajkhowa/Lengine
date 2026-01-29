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
        static inline std::string GameAssets_Prefab;
        static inline std::string GameScenes;

        static inline std::string GameAssetRegistryFolder;

        static inline std::string GameLibrary;
        static inline std::string GameLibrary_Assets;
        static inline std::string GameLibrary_Assets_Mesh;
        static inline std::string GameLibrary_Assets_Material;
        static inline std::string GameLibrary_Assets_Texture;
        static inline std::string GameLibrary_Assets_Prefab;


        static inline std::string GameAssetDatabase;



        static void setGameFolderPath(const std::string& gameFolderPath)
        {
            ActiveGameFolder = gameFolderPath;

            GameAssets = ActiveGameFolder + "/Assets/";
            GameAssetRegistryFolder = ActiveGameFolder + "AssetRegistry/";
            GameScenes = ActiveGameFolder + "/Scenes/";
            GameLibrary = ActiveGameFolder + "/Library/";



            GameAssets_Shaders = GameAssets + "Shaders/";
            GameAssets_Materials = GameAssets + "Materials/";
            GameAssets_Mesh = GameAssets + "Mesh/";
            GameAssets_Textures = GameAssets + "Textures/";
            GameAssets_Prefab = GameAssets + "Prefab/";


            GameLibrary_Assets = GameLibrary + "Assets/";
            GameLibrary_Assets_Mesh = GameLibrary_Assets + "Mesh/";
            GameLibrary_Assets_Material = GameLibrary_Assets + "Material/";
            GameLibrary_Assets_Texture = GameLibrary_Assets + "Texture/";
            GameLibrary_Assets_Prefab = GameLibrary_Assets + "Prefab/";


            GameAssetDatabase = ActiveGameFolder + "/AssetDatabase/";
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

        static inline std::string pbrVertexShaderPath = Vert("pbr");
        static inline std::string pbrFragmentShaderPath = Frag("pbr");
    };


}
