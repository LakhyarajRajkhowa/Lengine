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
        unsigned int index;
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;

        unsigned int materialIndex;

        

        
        glm::vec3 localCenter;
        float boundingRadius;

        void computeBounds();


    public:
        SubMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~SubMesh();

        bool isHovered = false;
        bool isSelected = false;
        bool isVisible = true;

        void draw() const;
        const glm::vec3& getLocalCenter() { return localCenter; }
        float& getBoundingRadius() { return boundingRadius; }
        void setupMesh();
        const std::string& getName() { return name; }
        const unsigned int& getIndex() { return index; }

        void setMatIdx(unsigned int idx) { materialIndex = idx; }
        void setIndex(unsigned int ID) { index = ID; }

        const unsigned int& getMatIdx() { return  materialIndex; }

    };
    class Mesh {
    public:
        std::string name;
        std::unordered_map<unsigned int, std::vector<unsigned int>> materialGroups;
        std::vector<SubMesh> subMeshes;

        std::vector<bool> visibleMaterialGroups ;
        SubMesh* Mesh::getSubMeshByIndex(const unsigned int& id) {
            for (auto& sm : subMeshes) {
                if (sm.getIndex() == id)
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

