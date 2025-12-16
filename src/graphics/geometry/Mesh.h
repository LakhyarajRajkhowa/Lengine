#pragma once
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../graphics/material/Material.h"
#include "../logging/LogBuffer.h"
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
        unsigned int id;
        std::string name;
        unsigned int materialIndex;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;

        
        glm::vec3 localCenter;
        float boundingRadius;

        void computeBounds();


    public:
        SubMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~SubMesh();

        bool isHovered = false;
        bool isSelected = false;

        void draw() const;
        const glm::vec3& getLocalCenter() { return localCenter; }
        float& getBoundingRadius() { return boundingRadius; }
        void setupMesh();
        const std::string& getName() { return name; }
        const unsigned int& getID() { return id; }

        void setMatIdx(unsigned int idx) { materialIndex = idx; }
        void setId(unsigned int ID) { id = ID; }

        const unsigned int& getMatIdx() { return  materialIndex; }

    };
    class Mesh {
    public:
        std::string name;
        std::unordered_map<unsigned int, std::vector<unsigned int>> materialGroups;
        std::vector<SubMesh> subMeshes;


        SubMesh* Mesh::getSubMeshByID(const unsigned int& id) {
            for (auto& sm : subMeshes) {
                if (sm.getID() == id)
                    return &sm;
            }
            return nullptr;
        }

        void draw() const {
            for (const auto& sm : subMeshes) {
                sm.draw();
            }
        }
    
    };
}

