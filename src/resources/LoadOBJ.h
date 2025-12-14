
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

    inline void loadOBJ(const std::string& path, Mesh& mesh, bool verbose = false) {
        // Read file into memory
        std::ifstream ifs(path, std::ios::binary | std::ios::ate);
        if (!ifs) {
            std::cerr << "Failed to open OBJ: " << path << "\n";
            return;
        }
        std::streamsize fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buf;
        buf.resize((size_t)fileSize + 1);
        if (!ifs.read(buf.data(), fileSize)) {
            std::cerr << "Failed to read OBJ: " << path << "\n";
            return;
        }
        buf[fileSize] = '\0';
        char* data = buf.data();
        char* end = data + fileSize;

        // Temporary storage
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec3> normals;

        // Reserve heuristic (helps a lot)
        size_t approxVerts = (size_t)std::max<int64_t>(1024, (int64_t)(fileSize / 60));
        positions.reserve(approxVerts);
        texcoords.reserve(approxVerts / 4);
        normals.reserve(approxVerts / 4);

        std::vector<Vertex> buildVerts;
        std::vector<unsigned int> buildIndices;
        buildVerts.reserve(approxVerts);
        buildIndices.reserve(approxVerts * 3);

        std::unordered_map<VertexKey, unsigned int, VertexKeyHasher> vertexCache;
        vertexCache.reserve(approxVerts * 2);

        auto pushCurrentSubMeshIfNotEmpty = [&]() {
            if (!buildVerts.empty() && !buildIndices.empty()) {
                mesh.subMeshes.emplace_back("defaultName", buildVerts, buildIndices);
                if (verbose) {
                    std::cout << "[FASTOBJ] Pushed submesh (verts=" << buildVerts.size()
                        << ", indices=" << buildIndices.size() << ")\n";
                }
            }
            else {
                if (verbose && (!buildVerts.empty() || !buildIndices.empty())) {
                    std::cout << "[FASTOBJ] WARNING: not pushing partial submesh: verts="
                        << buildVerts.size() << " indices=" << buildIndices.size() << "\n";
                }
            }
            buildVerts.clear();
            buildIndices.clear();
            vertexCache.clear();
            };

        // iterate lines via pointer
        char* lineStart = data;
        size_t lineNo = 0;
        while (lineStart < end) {
            ++lineNo;
            // find end of line
            char* lineEnd = (char*)memchr(lineStart, '\n', end - lineStart);
            if (!lineEnd) lineEnd = end;
            // skip leading spaces
            char* p = lineStart;
            while (p < lineEnd && (*p == ' ' || *p == '\t' || *p == '\r')) ++p;
            if (p >= lineEnd) { lineStart = lineEnd + 1; continue; }

            char c = *p;
            if (c == '#') { lineStart = lineEnd + 1; continue; } // comment

            // Vertex position 'v '
            if (c == 'v' && (p + 1 < lineEnd) && (p[1] == ' ' || p[1] == '\t')) {
                p += 1; // skip 'v'
                p = skipSpaces(p + 1, lineEnd);
                // parse three floats
                float x = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd);
                float y = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd);
                float z = parseFloat(p, lineEnd);
                positions.emplace_back(x, y, z);
            }
            // Texture coord 'vt'
            else if (c == 'v' && (p + 2 < lineEnd) && p[1] == 't' && (p[2] == ' ' || p[2] == '\t')) {
                p += 2;
                p = skipSpaces(p + 1, lineEnd);
                float u = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd);
                float v = parseFloat(p, lineEnd);
                texcoords.emplace_back(u, v);
            }
            // Normal 'vn'
            else if (c == 'v' && (p + 2 < lineEnd) && p[1] == 'n' && (p[2] == ' ' || p[2] == '\t')) {
                p += 2;
                p = skipSpaces(p + 1, lineEnd);
                float nx = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd);
                float ny = parseFloat(p, lineEnd);
                p = skipSpaces(p, lineEnd);
                float nz = parseFloat(p, lineEnd);
                normals.emplace_back(nx, ny, nz);
            }
            // new material start -> push submesh
            else if (c == 'u' && (p + 5 < lineEnd) && strncmp(p, "usemtl", 6) == 0) {
                pushCurrentSubMeshIfNotEmpty();
            }
            // group -> push
            else if (c == 'g' && (p + 1 < lineEnd) && (p[1] == ' ' || p[1] == '\t')) {
                pushCurrentSubMeshIfNotEmpty();
            }
            // faces
            else if (c == 'f' && (p + 1 < lineEnd) && (p[1] == ' ' || p[1] == '\t')) {
                p += 1;
                p = skipSpaces(p + 1, lineEnd);

                std::vector<unsigned int> faceIdxs;
                faceIdxs.reserve(8);

                while (p < lineEnd) {
                    // skip spaces
                    p = skipSpaces(p, lineEnd);
                    if (p >= lineEnd) break;
                    // token start
                    char* tokStart = p;
                    // find token end
                    while (p < lineEnd && *p != ' ' && *p != '\t' && *p != '\r') ++p;
                    char* tokEnd = p;

                    int vi = 0, vti = 0, vni = 0;
                    parseFaceToken(tokStart, tokEnd, vi, vti, vni);

                    // resolve negative indices
                    auto resolveIndex = [&](int idx, size_t containerSize) -> int {
                        if (idx > 0) return idx;
                        if (idx < 0) return (int)containerSize + idx + 1;
                        return 0;
                        };

                    int rv = resolveIndex(vi, positions.size());
                    int rvt = resolveIndex(vti, texcoords.size());
                    int rvn = resolveIndex(vni, normals.size());

                    if (rv <= 0 || rv > (int)positions.size()) {
                        std::cerr << "[FASTOBJ][Error] Line " << lineNo << " vertex index out of range v=" << vi << "\n";
                        // skip this face token
                        continue;
                    }
                    // build key
                    VertexKey key{ rv, rvt, rvn };
                    auto it = vertexCache.find(key);
                    if (it != vertexCache.end()) {
                        faceIdxs.push_back(it->second);
                    }
                    else {
                        Vertex vert{};
                        vert.position = positions[rv - 1];
                        vert.texCoord = (rvt > 0) ? texcoords[rvt - 1] : glm::vec2(0.0f);
                        vert.normal = (rvn > 0) ? normals[rvn - 1] : glm::vec3(0.0f, 1.0f, 0.0f);
                        vert.tangent = glm::vec3(0.0f);
                        vert.bitangent = glm::vec3(0.0f);

                        buildVerts.push_back(vert);
                        unsigned int idx = (unsigned int)buildVerts.size() - 1;
                        vertexCache.emplace(key, idx);
                        faceIdxs.push_back(idx);
                    }
                } // end face tokens

                if (faceIdxs.size() >= 3) {
                    for (size_t i = 1; i < faceIdxs.size() - 1; ++i) {
                        buildIndices.push_back(faceIdxs[0]);
                        buildIndices.push_back(faceIdxs[i]);
                        buildIndices.push_back(faceIdxs[i + 1]);
                    }
                }
                else {
                    if (verbose) std::cerr << "[FASTOBJ] face with <3 verts at line " << lineNo << "\n";
                }
            }

            // advance to next line
            lineStart = (lineEnd >= end) ? end : (lineEnd + 1);
        } // end while lines

        // push final submesh
        pushCurrentSubMeshIfNotEmpty();

        if (mesh.subMeshes.empty()) {
            std::cerr << "[FASTOBJ] Warning: no submeshes created for " << path << "\n";
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

       /* std::cout << mesh->mName.C_Str() << " -" << std::endl;
        std::cout << "Size of Vertices: "
            << (vertices.size() * sizeof(vertices[0])) / (1024.0 * 1024.0)
            << " MB\n";

        std::cout << "Size of Indices: "
            << (indices.size() * sizeof(indices[0])) / (1024.0 * 1024.0)
            << " MB\n";
        std::cout << std::endl;
        */

        return engineSubMesh;
    }

    inline int assimpLoader(const std::string& path, Mesh& mesh) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
            return -1;

        }

        std::cout << "Model loaded successfully! " << path << std::endl;

        if (scene && scene->mRootNode) {
            for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
                aiMesh* aMesh = scene->mMeshes[i];
                mesh.subMeshes.push_back(convertToEngineSubMesh(aMesh));

            }
        }

    }

} // namespace Lengine
