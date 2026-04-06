#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>

#include <glm/glm.hpp>

namespace Lengine {


    struct SkeletonBone
    {
        std::string name;
        int parentIndex;
        glm::mat4 inverseBindMatrix;
    };

    class Skeleton
    {
    public:

        Skeleton() = default;

        // metadata
        UUID skeletonID;
        std::string sourcePath;

        // bone data
        std::vector<SkeletonBone> bones;

        // name -> index lookup (very useful for animation)
        std::unordered_map<std::string, int> boneMap;

    public:

        inline int GetBoneIndex(const std::string& name) const
        {
            auto it = boneMap.find(name);
            if (it != boneMap.end())
                return it->second;

            return -1;
        }

        inline const SkeletonBone& GetBone(int index) const
        {
            return bones[index];
        }

        inline size_t GetBoneCount() const
        {
            return bones.size();
        }

        inline int GetParentIndex(int index) const
        {
            return bones[index].parentIndex;
        }
    };

    static std::shared_ptr<Skeleton> ReadSkeleton(const std::filesystem::path& path)
    {
        std::ifstream in(path, std::ios::binary);

        if (!in)
            throw std::runtime_error("Failed to open skeleton");

        auto skeleton = std::make_shared<Skeleton>();

        // UUID
        in.read((char*)&skeleton->skeletonID, sizeof(UUID));

        // source path
        uint32_t pathLen;
        in.read((char*)&pathLen, sizeof(uint32_t));

        skeleton->sourcePath.resize(pathLen);
        in.read(&skeleton->sourcePath[0], pathLen);

        // bone count
        uint32_t boneCount;
        in.read((char*)&boneCount, sizeof(uint32_t));

        skeleton->bones.resize(boneCount);


        for (uint32_t i = 0; i < boneCount; i++)
        {
            uint32_t nameLen;
            in.read((char*)&nameLen, sizeof(uint32_t));

            skeleton->bones[i].name.resize(nameLen);
            in.read(&skeleton->bones[i].name[0], nameLen);

            in.read((char*)&skeleton->bones[i].parentIndex, sizeof(int));
            in.read((char*)&skeleton->bones[i].inverseBindMatrix, sizeof(glm::mat4));

            skeleton->boneMap[skeleton->bones[i].name] = i;
        }

        return skeleton;
    }
}
