#pragma once
#include <glm/glm.hpp>
#include <string>
#include <optional>
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../utils/UUID.h"

namespace Lengine {
    class Material {
    public:

        glm::vec3 Kd = {0.5, 0.5, 0.5};        // DiffuseColor
        glm::vec3 Ka = { 0.05, 0.05, 0.05 };        // AmbientColor
        glm::vec3 Ks = { 3.50, 3.50, 3.50 };        // SpecularColor
        glm::vec3 Ke;        // EmissiveColor
        float Ns = 50.0f ;            // Shininess
        float d;             // Opacity
        float Ni;            // OpticalDensity
        float Tr;            // Transperancy
        float Tf;            // TransmissionFilter
        UUID map_Kd = UUID::Null;  // DiffuseMap
        UUID map_Ka = UUID::Null;  // AmbientMap
        UUID map_Ks = UUID::Null;  // SpecularMap
        UUID normalMap = UUID::Null;    // map_bump
        float metallic;
        float roughness;

        std::string name;
        GLSLProgram* shader = nullptr;

        Material(std::string matName, GLSLProgram* shaderProgram)
            : name(matName), shader(shaderProgram) {
        }

        GLSLProgram* getShader() const { return shader; }
        const std::string& getName() const { return  name; }
          

    };

    // MaterialInstance used by a Scene for each entity
    struct MaterialInstance {
        UUID baseMaterial;

        // Overrides (only stored if changed)
        std::optional<glm::vec3> Kd;
        std::optional<glm::vec3> Ka;
        std::optional<glm::vec3> Ks;
        std::optional<glm::vec3> Ke;
        std::optional<float> Ns;
        
        std::optional<UUID> map_kd;
        std::optional<UUID> map_ks;



    };

    // Final materiral send to GPU
    struct ResolvedMaterial {
        glm::vec3 Kd = { 0.5, 0.5, 0.5 };        // DiffuseColor
        glm::vec3 Ka = { 0.05, 0.05, 0.05 };        // AmbientColor
        glm::vec3 Ks = { 3.50, 3.50, 3.50 };        // SpecularColor
        glm::vec3 Ke;        // EmissiveColor
        float Ns = 50.0f;            // Shininess
        float d;             // Opacity
        float Ni;            // OpticalDensity
        float Tr;            // Transperancy
        float Tf;            // TransmissionFilter
        UUID map_Kd;  // DiffuseMap
        UUID map_Ka;  // AmbientMap
        UUID map_Ks = UUID::Null;  // SpecularMap
        UUID normalMap;    // map_bump
        float metallic;
        float roughness;

    };

}
