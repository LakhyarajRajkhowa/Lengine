#pragma once
#include <glm/glm.hpp>
#include <string>
#include <optional>
#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"
#include "../utils/UUID.h"

namespace Lengine {
    class PhongMaterial {
    public:
        PhongMaterial() = default;
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

        PhongMaterial(std::string matName, GLSLProgram* shaderProgram)
            : name(matName), shader(shaderProgram) {
        }

        GLSLProgram* getShader() const { return shader; }
        const std::string& getName() const { return  name; }
          

    };




    struct Material {
        Material() = default;

        UUID id = UUID::Null;
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

        Material(std::string matName, GLSLProgram* shaderProgram)
            : name(matName), shader(shaderProgram) {
        }

        GLSLProgram* getShader() const { return shader; }
        const std::string& getName() const { return  name; }

        bool localDirty = false;
    };

    struct ResolvedMaterial {

        glm::vec3 albedo;
        float metallic;
        float roughness;
        float ao;

        UUID map_albedo = UUID::Null;
        UUID map_normal = UUID::Null;
        UUID map_metallic = UUID::Null;
        UUID map_roughness = UUID::Null;
        UUID map_ao = UUID::Null;
        UUID map_metallicRoughness = UUID::Null;

        float normalStrength;
    };

    struct MaterialInstance {
        UUID baseMaterial = UUID::Null;
        bool dirty = true;
        ResolvedMaterial cached;

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



    static ResolvedMaterial ResolveMaterial(
        const Material& base,
        const MaterialInstance& inst)
    {
        ResolvedMaterial out;

        out.albedo = inst.albedo.value_or(base.albedo);
        out.metallic = inst.metallic.value_or(base.metallic);
        out.roughness = inst.roughness.value_or(base.roughness);
        out.ao = inst.ao.value_or(base.ao);

        out.map_albedo =
            (inst.use_map_albedo && inst.map_albedo)
            ? *inst.map_albedo
            : base.map_albedo;

        out.map_normal =
            (inst.use_map_normal && inst.map_normal)
            ? *inst.map_normal
            : base.map_normal;

        out.map_ao =
            (inst.use_map_ao && inst.map_ao)
            ? *inst.map_ao
            : base.map_ao;

        out.map_metallic =
            (inst.use_map_metallic && inst.map_metallic)
            ? *inst.map_metallic
            : base.map_metallic;

        out.map_roughness =
            (inst.use_map_roughness && inst.map_roughness)
            ? *inst.map_roughness
            : base.map_roughness;

        out.map_metallicRoughness =
            (inst.use_map_metallicRoughness && inst.map_metallicRoughness)
            ? *inst.map_metallicRoughness
            : base.map_metallicRoughness;


        out.normalStrength =
            inst.normalStrength.value_or(base.normalStrength);

        return out;
    }



}
