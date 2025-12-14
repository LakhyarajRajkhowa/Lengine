#pragma once
#include <string>
#include <filesystem>

namespace Lengine {

    struct Paths
    {
        
        static inline std::string Assets = "../assets/";
        static inline std::string Shaders = Assets + "shaders/";
        static inline std::string Materials = Assets + "Materials/";
        static inline std::string Textures = Assets + "Textures/";
        static inline std::string Mesh = Assets + "Mesh/";
        static inline std::string Scenes = Assets + "Scenes/";

        

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
