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
        Material::Material() = default;
        glm::vec3 Kd = {0.5, 0.5, 0.5};        // DiffuseColor
        glm::vec3 Ka = { 0.05, 0.05, 0.05 };        // AmbientColor
        glm::vec3 Ks = { 3.50, 3.50, 3.50 };        // SpecularColor
        glm::vec3 Ke = {0.0f, 0.0f, 0.0f};        // EmissiveColor
        float Ns = 50.0f ;            // Shininess
        float d;             // Opacity
        float Ni;            // OpticalDensity
        float Tr;            // Transperancy
        float Tf;            // TransmissionFilter
        UUID map_Kd = UUID::Null;  // DiffuseMap
        UUID map_Ka = UUID::Null;  // AmbientMap
        UUID map_Ks = UUID::Null;  // SpecularMap
        UUID map_bump = UUID::Null;    // NormalMap
        float normalStrength = 1.0f;
        float emissiveStrength = 1.0f;
        float metallic;
        float roughness;

        std::string map_Kd_path;  // DiffuseMap
        std::string map_Ka_path;  // AmbientMap
        std::string map_Ks_path;  // SpecularMap
        std::string map_bump_path;    // NormalMap

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
        std::optional<UUID> map_bump;
        std::optional<float> normalStrength;

        bool use_map_kd = true;
        bool use_map_ks = true;
        bool use_map_bump = true;


    };

    // Final materiral send to GPU
    struct ResolvedMaterial {
        glm::vec3 Kd;        // DiffuseColor
        glm::vec3 Ka;        // AmbientColor
        glm::vec3 Ks;        // SpecularColor
        glm::vec3 Ke;        // EmissiveColor
        float Ns;            // Shininess
        float d;             // Opacity
        float Ni;            // OpticalDensity
        float Tr;            // Transperancy
        float Tf;            // TransmissionFilter
        UUID map_Kd = UUID::Null;  // DiffuseMap
        UUID map_Ka = UUID::Null;  // AmbientMap
        UUID map_Ks = UUID::Null;  // SpecularMap
        UUID map_bump = UUID::Null;    // NormalMap
        float normalStrength;

        float metallic;
        float roughness;

    };

}
