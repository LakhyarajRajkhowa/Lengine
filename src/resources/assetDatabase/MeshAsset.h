#pragma once

#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include "../utils/UUID.h" // your UUID class
#include "../graphics/geometry/Mesh.h" // Vertex struct, AABB struct
#include "../graphics/geometry/Bounds.h"



namespace Lengine {
    

    struct LMeshSubMeshRef
    {
        UUID subMeshID;
        uint32_t subMeshIndex;
        std::filesystem::path subMeshPath; // relative path to .lsubmesh
    };

    struct LMeshFile
    {
        UUID meshID;
        std::string sourcePath;   // original imported file (gltf/obj/fbx)

        AABB bounds;              // whole mesh bounds

        std::vector<LMeshSubMeshRef> subMeshes;
    };

    static void WriteLMesh(const std::filesystem::path& outPath, const LMeshFile& mesh)
    {
        std::ofstream out(outPath, std::ios::binary);
        if (!out)
            throw std::runtime_error("Failed to open .lmesh for writing");

        // Mesh UUID
        out.write((char*)&mesh.meshID, sizeof(UUID));

        // Source path
        uint32_t pathLen = (uint32_t)mesh.sourcePath.size();
        out.write((char*)&pathLen, sizeof(uint32_t));
        out.write(mesh.sourcePath.c_str(), pathLen);

        // Bounds
        out.write((char*)&mesh.bounds, sizeof(AABB));

        // Submesh refs
        uint32_t subMeshCount = (uint32_t)mesh.subMeshes.size();
        out.write((char*)&subMeshCount, sizeof(uint32_t));

        for (const auto& sm : mesh.subMeshes)
        {
            out.write((char*)&sm.subMeshID, sizeof(UUID));
            out.write((char*)&sm.subMeshIndex, sizeof(uint32_t));

            uint32_t smPathLen = (uint32_t)sm.subMeshPath.string().size();
            out.write((char*)&smPathLen, sizeof(uint32_t));
            out.write(sm.subMeshPath.string().c_str(), smPathLen);
        }
    }

    static LMeshFile ReadLMesh(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::cout << "Could not find file: " << path << std::endl;
        }

        LMeshFile mesh;
        std::ifstream in(path, std::ios::binary);
        if (!in)
            throw std::runtime_error("Failed to open .lmesh for reading");

        // UUID
        in.read((char*)&mesh.meshID, sizeof(UUID));

        // Source path
        uint32_t pathLen;
        in.read((char*)&pathLen, sizeof(uint32_t));
        mesh.sourcePath.resize(pathLen);
        in.read(mesh.sourcePath.data(), pathLen);

        // Bounds
        in.read((char*)&mesh.bounds, sizeof(AABB));

        // Submeshes
        uint32_t subMeshCount;
        in.read((char*)&subMeshCount, sizeof(uint32_t));
        mesh.subMeshes.resize(subMeshCount);

        for (auto& sm : mesh.subMeshes)
        {
            in.read((char*)&sm.subMeshID, sizeof(UUID));
            in.read((char*)&sm.subMeshIndex, sizeof(uint32_t));

            uint32_t smPathLen;
            in.read((char*)&smPathLen, sizeof(uint32_t));
            std::string pathStr(smPathLen, '\0');
            in.read(pathStr.data(), smPathLen);
            sm.subMeshPath = pathStr;
        }

        return mesh;
    }


    struct LSubMeshFile
    {
        UUID subMeshID;
        UUID parentMeshID;
        std::string sourcePath;
        uint32_t subMeshIndex;

        uint32_t vertexCount;
        uint32_t indexCount;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        AABB bounds;

        std::vector<int> bonePalette;


    };

    static void WriteLSubMesh(const std::filesystem::path& outPath, const LSubMeshFile& submesh)
    {


        std::ofstream out(outPath, std::ios::binary);
        if (!out) {
            std::cout << "Failed to open .lsubmesh file for writing: " << outPath << std::endl;
            return;

        }

        // --- Write UUIDs ---
        out.write((char*)&submesh.subMeshID, sizeof(UUID));
        out.write((char*)&submesh.parentMeshID, sizeof(UUID));



        // --- Write sourcePath ---
        uint32_t pathLen = (uint32_t)submesh.sourcePath.size();
        out.write((char*)&pathLen, sizeof(uint32_t));
        out.write(submesh.sourcePath.c_str(), pathLen);

        // --- Write subMeshIndex ---
        out.write((char*)&submesh.subMeshIndex, sizeof(uint32_t));

        // --- Write vertices ---
        uint32_t vertexCount = (uint32_t)submesh.vertices.size();
        out.write((char*)&vertexCount, sizeof(uint32_t));
        if (vertexCount > 0)
            out.write((char*)submesh.vertices.data(), vertexCount * sizeof(Vertex));

        // --- Write indices ---
        uint32_t indexCount = (uint32_t)submesh.indices.size();
        out.write((char*)&indexCount, sizeof(uint32_t));
        if (indexCount > 0)
            out.write((char*)submesh.indices.data(), indexCount * sizeof(uint32_t));

        // --- Write bounding box ---
        out.write((char*)&submesh.bounds, sizeof(AABB));

        // --- Write bone palette ---
        uint32_t paletteSize = (uint32_t)submesh.bonePalette.size();
        out.write((char*)&paletteSize, sizeof(uint32_t));

        if (paletteSize > 0)
            out.write((char*)submesh.bonePalette.data(), paletteSize * sizeof(int));

        out.close();
    }

    static LSubMeshFile ReadLSubMesh(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            std::cout << "Could not find file: " << path << std::endl;
        }

        LSubMeshFile submesh;
        std::ifstream in(path, std::ios::binary);
        if (!in)
            throw std::runtime_error("Failed to open .lsubmesh for reading");

        // UUIDs
        in.read((char*)&submesh.subMeshID, sizeof(UUID));
        in.read((char*)&submesh.parentMeshID, sizeof(UUID));

        // Source path
        uint32_t pathLen;
        in.read((char*)&pathLen, sizeof(uint32_t));
        submesh.sourcePath.resize(pathLen);
        in.read(submesh.sourcePath.data(), pathLen);

        // Submesh index
        in.read((char*)&submesh.subMeshIndex, sizeof(uint32_t));

        // Vertices
        uint32_t vertexCount;
        in.read((char*)&vertexCount, sizeof(uint32_t));
        submesh.vertices.resize(vertexCount);
        if (vertexCount > 0)
            in.read((char*)submesh.vertices.data(), vertexCount * sizeof(Vertex));

        // Indices
        uint32_t indexCount;
        in.read((char*)&indexCount, sizeof(uint32_t));
        submesh.indices.resize(indexCount);
        if (indexCount > 0)
            in.read((char*)submesh.indices.data(), indexCount * sizeof(uint32_t));

        // AABB
        in.read((char*)&submesh.bounds, sizeof(AABB));

        // --- Read bone palette ---
        uint32_t paletteSize;
        in.read((char*)&paletteSize, sizeof(uint32_t));

        submesh.bonePalette.resize(paletteSize);
        if (paletteSize > 0)
            in.read((char*)submesh.bonePalette.data(), paletteSize * sizeof(int));

        return submesh;
    }

    struct LSkeletonBone
    {
        std::string name;
        int parentIndex;
        glm::mat4 inverseBindMatrix;
    };

    struct LSkeletonFile
    {
        UUID skeletonID;
        std::string sourcePath;

        std::vector<LSkeletonBone> bones;
    };



    static void WriteSkeleton(
        const std::filesystem::path& outPath,
        const LSkeletonFile& skeleton)
    {
        std::ofstream out(outPath, std::ios::binary);

        if (!out)
        {
            std::cout << "Failed to write skeleton: " << outPath << std::endl;
            return;
        }

        // UUID
        out.write((char*)&skeleton.skeletonID, sizeof(UUID));

        // source path
        size_t pathLen = skeleton.sourcePath.size();
        out.write((char*)&pathLen, sizeof(uint32_t));
        out.write(skeleton.sourcePath.c_str(), pathLen);

        // bone count
        size_t boneCount = skeleton.bones.size();
        out.write((char*)&boneCount, sizeof(uint32_t));

        for (const auto& bone : skeleton.bones)
        {
            size_t nameLen = bone.name.size();

            out.write((char*)&nameLen, sizeof(uint32_t));
            out.write(bone.name.c_str(), nameLen);

            out.write((char*)&bone.parentIndex, sizeof(int));
            out.write((char*)&bone.inverseBindMatrix, sizeof(glm::mat4));
        }

        out.close();
    }

    struct LKeyPosition
    {
        glm::vec3 position;
        float time;
    };

    struct LKeyRotation
    {
        glm::quat rotation;
        float time;
    };

    struct LKeyScale
    {
        glm::vec3 scale;
        float time;
    };

    struct LAnimationTrack
    {
        std::string boneName;

        int boneIndex = -1;

        std::vector<LKeyPosition> positions;
        std::vector<LKeyRotation> rotations;
        std::vector<LKeyScale> scales;
    };

    struct LAnimationFile
    {
        UUID animationID;
        UUID skeletonID;

        std::string name;

        float duration;
        float ticksPerSecond;

        std::vector<LAnimationTrack> tracks;
        std::vector<int> boneTrackMap;
    };

    static void WriteLAnimation(const std::filesystem::path& path, const LAnimationFile& anim)
    {
    


        std::ofstream out(path, std::ios::binary);
        
        if (!out) {
            std::cout << "Failed to open .lanim file for writing :"<< path <<std::endl;
        }
        else {
            std::cout << "Writing file  :" << path << std::endl;
        }
        


        out.write((char*)&anim.animationID, sizeof(UUID));
        out.write((char*)&anim.skeletonID, sizeof(UUID));

        size_t nameLen = anim.name.size();
        out.write((char*)&nameLen, sizeof(uint32_t));
        out.write(anim.name.data(), nameLen);

        out.write((char*)&anim.duration, sizeof(float));
        out.write((char*)&anim.ticksPerSecond, sizeof(float));

        size_t trackCount = anim.tracks.size();
        out.write((char*)&trackCount, sizeof(uint32_t));




        for (const auto& track : anim.tracks)
        {
            size_t boneLen = track.boneName.size();
            out.write((char*)&boneLen, sizeof(uint32_t));
            out.write(track.boneName.data(), boneLen);

            size_t boneIdx = track.boneIndex;
            out.write((char*)&boneIdx, sizeof(uint32_t));

            size_t posCount = track.positions.size();
            out.write((char*)&posCount, sizeof(uint32_t));
            out.write((char*)track.positions.data(), posCount * sizeof(LKeyPosition));

            size_t rotCount = track.rotations.size();
            out.write((char*)&rotCount, sizeof(uint32_t));
            out.write((char*)track.rotations.data(), rotCount * sizeof(LKeyRotation));

            size_t scaleCount = track.scales.size();
            out.write((char*)&scaleCount, sizeof(uint32_t));
            out.write((char*)track.scales.data(), scaleCount * sizeof(LKeyScale));
        }

        size_t mapSize = anim.boneTrackMap.size();
        out.write((char*)&mapSize, sizeof(uint32_t));
        out.write((char*)anim.boneTrackMap.data(), mapSize * sizeof(int));
    }


}

