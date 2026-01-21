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

    struct PBRMaterial {
        PBRMaterial::PBRMaterial() = default;

        glm::vec3 albedo = glm::vec3(1.0f);
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;

        std::string map_albedo_path;
        std::string map_normal_path;
        std::string map_metallic_path;
        std::string map_roughness_path;
        std::string map_ao_path;
        std::string map_metallicRoughness_path;

        UUID map_albedo = UUID::Null;
        UUID map_normal = UUID::Null;
        UUID map_metallic = UUID::Null;
        UUID map_roughness = UUID::Null;
        UUID map_ao = UUID::Null;
        UUID map_metallicRoughness = UUID::Null;


        float normalStrength = 1.0f;

        std::string name;
        GLSLProgram* shader = nullptr;

        PBRMaterial(std::string matName, GLSLProgram* shaderProgram)
            : name(matName), shader(shaderProgram) {
        }

        GLSLProgram* getShader() const { return shader; }
        const std::string& getName() const { return  name; }
    };


    struct PBRMaterialInstance {
        UUID baseMaterial;

        std::optional<glm::vec3> albedo;
        std::optional<float> metallic;
        std::optional<float> roughness;
        std::optional<float> ao;

        std::optional<UUID> map_albedo;
        std::optional<UUID> map_normal;
        std::optional<UUID> map_metallic;
        std::optional<UUID> map_roughness;
        std::optional<UUID> map_ao;
        std::optional<UUID> map_metallicRoughness;


        std::optional<float> normalStrength;

        bool use_map_albedo = true;
        bool use_map_normal = true;
        bool use_map_metallic = true;
        bool use_map_roughness = true;
        bool use_map_ao = true;
        bool use_map_metallicRoughness = true;

    };

    struct ResolvedPBRMaterial {

        glm::vec3 albedo ;
        float metallic ;
        float roughness;
        float ao ;

        UUID map_albedo = UUID::Null;
        UUID map_normal = UUID::Null;
        UUID map_metallic = UUID::Null;
        UUID map_roughness = UUID::Null;
        UUID map_ao = UUID::Null;
        UUID map_metallicRoughness = UUID::Null;

        float normalStrength;
    };





}
