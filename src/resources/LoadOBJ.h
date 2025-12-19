
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../graphics/geometry/Mesh.h"

#include "../scene/EntityComponentSystem.h"


namespace Lengine {

    struct MeshProperties {
        bool hasMaterials = false;
        bool hasAnimations = false;
        bool hasCamera = false;

    };


    inline SubMesh convertToEngineSubMesh(const aiMesh* mesh) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(mesh->mNumVertices); 
        indices.reserve(mesh->mNumFaces * 3);

        // --- Convert vertices ---
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};

            // Position
            if (mesh->HasPositions() && mesh->mVertices)
                vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            else
                vertex.position = glm::vec3(0.0f);

            // Normal
            if (mesh->HasNormals() && mesh->mNormals)
                vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            else
                vertex.normal = glm::vec3(0.0f);

            // Texture coordinates (only first UV channel)
            if (mesh->HasTextureCoords(0) && mesh->mTextureCoords[0] ) {
                vertex.texCoord = glm::vec2( // TODO: Fix Access Violation error when loading some mesh
                    mesh->mTextureCoords[0][i].x, 
                    mesh->mTextureCoords[0][i].y
                );
            }
            else {
                vertex.texCoord = glm::vec2(0.0f);
            }


            // Tangents and Bitangents
            if (mesh->HasTangentsAndBitangents() && mesh->mTangents && mesh->mBitangents) {
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
            else {
                vertex.tangent = glm::vec3(0.0f);
                vertex.bitangent = glm::vec3(0.0f);
            }

            vertices.push_back(vertex);
        }

        // --- Convert faces (indices) ---
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            // Defensive check � only handle triangles
            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }
        std::string name = mesh->mName.C_Str();

        return SubMesh(name, std::move(vertices), std::move(indices));
    }

    inline MeshProperties assimpLoader(
        const std::string& path,
        Mesh& mesh
    ) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

        MeshProperties properties;

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
            return properties;
        }

        std::cout << "Model loaded successfully! " << path << std::endl;

        // additional info in the mesh file
        properties.hasMaterials = scene->HasMaterials();


        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* aMesh = scene->mMeshes[i];
            SubMesh submesh = convertToEngineSubMesh(aMesh);

            if (properties.hasMaterials) {
                submesh.setMatIdx(aMesh->mMaterialIndex);
                submesh.setIndex(i);
                mesh.subMeshes.emplace_back(std::move(submesh));

                mesh.materialGroups[aMesh->mMaterialIndex].push_back(submesh.getIndex());
                mesh.visibleMaterialGroups.push_back(true);
            }
        }

        return properties;

    }

}