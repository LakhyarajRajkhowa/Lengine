#include "Mesh.h"
#include <iostream>

namespace Lengine {
    Mesh::Mesh(
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

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Mesh::computeBounds() {
        if (vertices.empty()) return;

        aabbMin = vertices[0].position;
        aabbMax = vertices[0].position;

        for (const auto& v : vertices) {
            aabbMin = glm::min(aabbMin, v.position);
            aabbMax = glm::max(aabbMax, v.position);
        }

        localCenter = (aabbMin + aabbMax) * 0.5f;

        float maxDist = 0.0f;
        for (const auto& v : vertices) {
            maxDist = glm::max(maxDist, glm::length(v.position - localCenter));
        }

        boundingRadius = maxDist;
    }

    

    void Mesh::setupMesh() {

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

        // Bone IDs
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

        // Bone Weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, weights));
        


        glBindVertexArray(0);

    }

    void Mesh::draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::reserve(uint32_t vCount, uint32_t iCount) {
        vertices.reserve(vCount);
        indices.reserve(iCount);
    }

    void Mesh::addVertex(const Vertex& v) {
        vertices.push_back(v);
    }

    void Mesh::addIndex(uint32_t i) {
        indices.push_back(i);
    }


   
}
