#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../graphics/material/Material.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};


namespace Lengine {
    class SubMesh {
    private:
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;

        
        glm::vec3 localCenter;
        float boundingRadius;

        void computeBounds();


    public:
        SubMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~SubMesh();

        bool isSelected = false;

        void draw() const;
        const glm::vec3& getLocalCenter() { return localCenter; }
        float& getBoundingRadius() { return boundingRadius; }
        void setupMesh();
        const std::string& getName() { return name; }
    };
    class Mesh {
    public:
        std::string name;
        std::vector<SubMesh> subMeshes;

        void draw() const {
            for (const auto& sm : subMeshes)
                sm.draw();
        }
    
    };
}

