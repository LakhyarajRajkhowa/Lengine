#pragma once
#include <glm/glm.hpp>
#include <string>

#include "../graphics/opengl/GLSLProgram.h"
#include "../graphics/opengl/GLTexture.h"

namespace Lengine {
    class Material {
    private:
        
        glm::vec3 diffuseColor;   // Kd
        glm::vec3 ambientColor;   // Ka
        glm::vec3 specularColor;  // Ks
        float shininess;          // Ns
        std::string diffuseMap;   // map_Kd
        std::string normalMap;    // map_bump

        
        
        float metallic = 0.0f;
        float roughness = 0.0f;

    public:
        std::string name;
        GLSLProgram* shader = nullptr;
        GLTexture* albedoTexture{};

        glm::vec3 objectColor = glm::vec3(0.54, 0.54, 0.54);

        Material(std::string matName, GLSLProgram* shaderProgram)
            : name(matName), shader(shaderProgram) {
        }

        GLSLProgram* getShader() const { return shader; }
        const std::string& getName() const { return  name; }
        bool useTexture = false;

        void apply() {

            shader->setFloat("metallic", metallic);
            shader->setFloat("roughness", roughness);
            shader->setVec3("objectColor", objectColor);
            if (useTexture && albedoTexture->id != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, albedoTexture->id);
                shader->setInt("albedoMap", 0);
                shader->setInt("useTexture", 1);
            }
            else {
                shader->setInt("useTexture", 0);
            }
        }

    };
}
