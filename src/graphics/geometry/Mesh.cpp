#include "Mesh.h"
#include <iostream>

namespace Lengine {
    SubMesh::SubMesh(
        const std::string& name,
        std::vector<Vertex>&& verts,
        std::vector<uint32_t>&& inds
    )
        : name(name),
        vertices(std::move(verts)),
        indices(std::move(inds))
   
    {
        computeBounds();
        setupMesh();

    }

    SubMesh::~SubMesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void SubMesh::computeBounds() {
        if (vertices.empty()) return;

        glm::vec3 minV = vertices[0].position;
        glm::vec3 maxV = vertices[0].position;

        // Step 1: find bounding box
        for (const auto& v : vertices) {
            minV = glm::min(minV, v.position);
            maxV = glm::max(maxV, v.position);
        }

        // Step 2: center = middle of min and max
        glm::vec3 size = maxV - minV;
        localCenter = (minV + maxV) * 0.5f;

        //adjusting the size
        float maxExtent = glm::max(size.x, glm::max(size.y, size.z));

        // Step 3: bounding radius = max distance from center
        float maxDist = 0.0f;
        for (const auto& v : vertices) {
            maxDist = glm::max(maxDist, glm::length(v.position - localCenter));
        }

        boundingRadius = maxDist;

    }
    

    void SubMesh::setupMesh() {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
            vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); 

        // TexCoord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        
        // Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, tangent)
        );

        // Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, bitangent)
        );

        
        glBindVertexArray(0);

    }

    void SubMesh::draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void SubMesh::reserve(uint32_t vCount, uint32_t iCount) {
        vertices.reserve(vCount);
        indices.reserve(iCount);
    }

    void SubMesh::addVertex(const Vertex& v) {
        vertices.push_back(v);
    }

    void SubMesh::addIndex(uint32_t i) {
        indices.push_back(i);
    }
   
}
