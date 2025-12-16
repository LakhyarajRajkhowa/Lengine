
/*
    I DONT KNOW HOW THIS SHIT WORKS
        (made with chatgpt)
*/

// fast_obj_loader.h 
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <cstdlib> // strtof/strtol
#include <sys/stat.h> // optional for file timestamp
#include <glm/glm.hpp>
#include <sstream>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../graphics/geometry/Mesh.h"


namespace Lengine {

    struct VertexKey {
        int v, vt, vn;
        bool operator==(const VertexKey& o) const { return v == o.v && vt == o.vt && vn == o.vn; }
    };
    struct VertexKeyHasher {
        size_t operator()(VertexKey const& k) const noexcept {
            // simple mix, avoid heavy operations
            return ((uint64_t)k.v * 73856093u) ^ ((uint64_t)k.vt * 19349663u) ^ ((uint64_t)k.vn * 83492791u);
        }
    };

    // Fast helper: skip spaces
    inline char* skipSpaces(char* p, char* end) {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\r')) ++p;
        return p;
    }

    // Parse a float from p (using strtof). Advances p to endptr and returns value.
    inline float parseFloat(char*& p, char* end) {
        // strtof expects NUL-terminated string; but it stops at non-numeric char and sets endptr.
        char* e;
        float v = strtof(p, &e);
        if (e == p) { // no progress: return 0 and advance to end or next space
            // fallback: skip token
            while (p < end && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') ++p;
            return 0.0f;
        }
        p = e;
        return v;
    }

    // Parse an integer (base 10) via strtol; advances p
    inline int parseInt(char*& p, char* end) {
        char* e;
        long v = strtol(p, &e, 10);
        if (e == p) {
            while (p < end && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') ++p;
            return 0;
        }
        p = e;
        return (int)v;
    }

    // Parse signed int token that may be part of a face token (like "12/34/56" or "-1//2")
    // Input tokenStart..tokenEnd (not null-terminated). We'll parse by temporarily setting a null.
    inline void parseFaceToken(char* tokenStart, char* tokenEnd, int& outV, int& outVT, int& outVN) {
        // Copy to small stack buffer and null-terminate (tokens are small)
        int len = int(tokenEnd - tokenStart);
        if (len <= 0) { outV = outVT = outVN = 0; return; }
        // Most tokens are short (<64), safe to stack-alloc small buffer
        char tmp[64];
        if (len >= (int)sizeof(tmp)) len = sizeof(tmp) - 1;
        memcpy(tmp, tokenStart, len);
        tmp[len] = '\0';

        // Now parse with manual scanning (fast)
        // Cases: v, v/vt, v//vn, v/vt/vn
        char* s = tmp;
        char* a = strchr(s, '/');
        if (!a) {
            outV = (int)strtol(s, nullptr, 10);
            outVT = outVN = 0;
            return;
        }
        // there's at least one slash
        *a = '\0';
        outV = (int)strtol(s, nullptr, 10);
        char* b = a + 1;
        char* c = strchr(b, '/');
        if (!c) {
            // v/vt
            outVT = (int)strtol(b, nullptr, 10);
            outVN = 0;
            return;
        }
        // v//vn or v/vt/vn
        if (c == b) {
            // v//vn -> vt missing
            outVT = 0;
            outVN = (int)strtol(c + 1, nullptr, 10);
            return;
        }
        else {
            // v/vt/vn
            *c = '\0';
            outVT = (int)strtol(b, nullptr, 10);
            outVN = (int)strtol(c + 1, nullptr, 10);
            return;
        }
    }
    inline void loadOBJ(const std::string& path, Mesh& mesh, bool verbose = false)
    {
        // --- Read file into memory ---
        std::ifstream ifs(path, std::ios::binary | std::ios::ate);
        if (!ifs) { std::cerr << "Failed to open OBJ: " << path << "\n"; return; }
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::vector<char> buf(size + 1);
        if (!ifs.read(buf.data(), size)) { std::cerr << "Failed to read OBJ: " << path << "\n"; return; }
        buf[size] = '\0';
        char* data = buf.data();
        char* end = data + size;

        // --- Temporary storage ---
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec3> normals;

        // material -> SubMeshBuilder
        struct SubMeshBuilder {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::unordered_map<VertexKey, unsigned int, VertexKeyHasher> cache;
        };
        std::unordered_map<std::string, SubMeshBuilder> materialBuckets;

        std::string currentMaterial = "default";

        // --- Line parsing ---
        char* lineStart = data;
        size_t lineNo = 0;

        while (lineStart < end)
        {
            ++lineNo;
            char* lineEnd = (char*)memchr(lineStart, '\n', end - lineStart);
            if (!lineEnd) lineEnd = end;

            char* p = lineStart;
            while (p < lineEnd && (*p == ' ' || *p == '\t' || *p == '\r')) ++p;
            if (p >= lineEnd) { lineStart = lineEnd + 1; continue; }

            char c = *p;

            // ---------------- COMMENT ----------------
            if (c == '#') { lineStart = lineEnd + 1; continue; }

            // ---------------- VERTEX ----------------
            if (c == 'v' && (p + 1 < lineEnd) && (p[1] == ' ' || p[1] == '\t')) {
                p += 1; p = skipSpaces(p, lineEnd);
                float x = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd); float y = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd); float z = parseFloat(p, lineEnd);
                positions.emplace_back(x, y, z);
            }
            else if (c == 'v' && (p + 2 < lineEnd) && p[1] == 't') {
                p += 2; p = skipSpaces(p, lineEnd);
                float u = parseFloat(p, lineEnd); p = skipSpaces(p, lineEnd);
                float v = parseFloat(p, lineEnd);
                texcoords.emplace_back(u, v);
            }
            else if (c == 'v' && (p + 2 < lineEnd) && p[1] == 'n') {
                p += 2; p = skipSpaces(p, lineEnd);
                float nx = parseFloat(p, lineEnd); p = skipSpaces(p, lineEnd);
                float ny = parseFloat(p, lineEnd); p = skipSpaces(p, lineEnd);
                float nz = parseFloat(p, lineEnd);
                normals.emplace_back(nx, ny, nz);
            }

            // ---------------- MATERIAL ----------------
            else if (c == 'u' && strncmp(p, "usemtl", 6) == 0) {
                p += 6; p = skipSpaces(p, lineEnd);
                char* nameStart = p;
                while (p < lineEnd && *p != ' ' && *p != '\t' && *p != '\r') ++p;
                currentMaterial = std::string(nameStart, p);
                if (currentMaterial.empty()) currentMaterial = "default";
            }

            // ---------------- FACE ----------------
            else if (c == 'f' && (p + 1 < lineEnd)) {
                p += 1; p = skipSpaces(p, lineEnd);
                auto& bucket = materialBuckets[currentMaterial];
                std::vector<unsigned int> faceIndices;

                while (p < lineEnd) {
                    p = skipSpaces(p, lineEnd);
                    if (p >= lineEnd) break;

                    char* tokEnd = p;
                    while (tokEnd < lineEnd && *tokEnd != ' ' && *tokEnd != '\t' && *tokEnd != '\r') ++tokEnd;

                    int vi = 0, vti = 0, vni = 0;
                    parseFaceToken(p, tokEnd, vi, vti, vni);

                    auto resolve = [&](int idx, size_t sz) { return (idx > 0) ? idx - 1 : (int)sz + idx; };
                    VertexKey key{ resolve(vi, positions.size()), resolve(vti, texcoords.size()), resolve(vni, normals.size()) };

                    auto it = bucket.cache.find(key);
                    if (it != bucket.cache.end())
                        faceIndices.push_back(it->second);
                    else {
                        Vertex v{};
                        v.position = positions[key.v];
                        if (key.vt >= 0) v.texCoord = texcoords[key.vt];
                        if (key.vn >= 0) v.normal = normals[key.vn];

                        bucket.vertices.push_back(v);
                        unsigned int idx = (unsigned int)bucket.vertices.size() - 1;
                        bucket.cache[key] = idx;
                        faceIndices.push_back(idx);
                    }

                    p = tokEnd;
                }

                // triangulate polygon
                for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
                    bucket.indices.push_back(faceIndices[0]);
                    bucket.indices.push_back(faceIndices[i]);
                    bucket.indices.push_back(faceIndices[i + 1]);
                }
            }

            // ---------------- NEXT LINE ----------------
            lineStart = (lineEnd >= end) ? end : (lineEnd + 1);
        }

        // ---------------- BUILD SUBMESHES ----------------
        for (auto& [matName, builder] : materialBuckets) {
            
            if (builder.vertices.empty() || builder.indices.empty()) continue;
            SubMesh sm(matName, builder.vertices, builder.indices);
            mesh.subMeshes.push_back(std::move(sm));
            std::cout << matName << std::endl;
            builder.vertices.clear();
            builder.indices.clear();
            builder.cache.clear();
        }

        if (verbose) {
            std::cout << "[FASTOBJ] Loaded " << mesh.subMeshes.size() << " submeshes from " << path << "\n";
        }
    }



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
            if (mesh->HasTextureCoords(0) && mesh->mTextureCoords[0] && i < mesh->mTextureCoords[0]->Length()) {
                vertex.texCoord = glm::vec2(
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
            // Defensive check — only handle triangles
            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }
        std::string name = mesh->mName.C_Str();
        // Construct SubMesh
        SubMesh engineSubMesh(name, vertices, indices);

        //clear temp memory
        std::vector<Vertex>().swap(vertices);
        std::vector<uint32_t>().swap(indices);
        // print mesh size 

      

        return engineSubMesh;
    }

    inline void assimpLoader(const std::string& path, Mesh& mesh) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
            return;
        }

        std::cout << "Model loaded successfully! " << path << std::endl;

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* aMesh = scene->mMeshes[i];
            SubMesh submesh = convertToEngineSubMesh(aMesh);
            submesh.setMatIdx(aMesh->mMaterialIndex);
            submesh.setId(i);
            mesh.subMeshes.push_back(std::move(submesh));
            mesh.materialGroups[aMesh->mMaterialIndex].push_back(submesh.getID());
            std::cout << aMesh->mMaterialIndex << " , " << submesh.getID() << std::endl;
        }
    }


} // namespace Lengine
