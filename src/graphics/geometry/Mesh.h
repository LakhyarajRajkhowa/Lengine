#pragma once
#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../graphics/material/Material.h"
#include "../logging/LogBuffer.h"

#include "../graphics/geometry/Bounds.h"



namespace Lengine {
    class Submesh {
    private:

        std::string name;
        std::vector<unsigned int> indices;
        GLuint VAO, VBO, EBO;

        void computeBounds();


    public:
        std::vector<Vertex> vertices;

        Submesh::Submesh() = default;
        Submesh(
            const std::string& name,
            std::vector<Vertex>&& verts,
            std::vector<uint32_t>&& inds
        );
      
        ~Submesh();



        glm::vec3 aabbMin;
        glm::vec3 aabbMax;
        glm::vec3 localCenter;
        float boundingRadius;
        void draw() const;
        const glm::vec3& getLocalCenter() { return localCenter; }
        float& getBoundingRadius() { return boundingRadius; }
        void setupMesh();
        const std::string& getName() { return name; }

        void reserve(uint32_t vertexCount, uint32_t indexCount);
        void addVertex(const Vertex& v);
        void addIndex(uint32_t i);



    };
    class Mesh {
    public:
        
        std::string name;
        std::string path;
        std::unordered_map<unsigned int, std::vector<unsigned int>> materialGroups;
        std::vector<Submesh> subMeshes;

        std::vector<bool> visibleMaterialGroups ;
       

        void draw() const {
            for (const auto& sm : subMeshes) {
                sm.draw();
            }
        }
        
        glm::vec3 aabbMin;
        glm::vec3 aabbMax;

        glm::vec3 localCenter;
        float boundingRadius;

        void computeBounds();
        const glm::vec3& getLocalCenter() { return localCenter; }
        float& getBoundingRadius() { return boundingRadius; }

        uint32_t materialSlotCount = 0;
    };
}

