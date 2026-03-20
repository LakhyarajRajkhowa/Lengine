#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../utils/UUID.h"

namespace Lengine {

    struct AnimationKeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct AnimationKeyRotation
    {
        glm::quat rotation;
        float timeStamp;
    };

    struct AnimationKeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    struct AnimationTrack
    {
        std::string boneName;

        int boneIndex = -1; // mapped to skeleton

        std::vector<AnimationKeyPosition> positions;
        std::vector<AnimationKeyRotation> rotations;
        std::vector<AnimationKeyScale> scales;
    };

    class Animation
    {
    public:

        UUID animationID;
        UUID skeletonID;

        std::string name;

        float duration = 0.0f;
        float ticksPerSecond = 0.0f;

        std::vector<AnimationTrack> tracks;

    };

    static std::shared_ptr<Animation> ReadAnimation(const std::filesystem::path& path)
    {
        std::ifstream in(path, std::ios::binary);

        if (!in)
            throw std::runtime_error("Failed to open animation");

        auto animation = std::make_shared<Animation>();

        // UUID
        in.read((char*)&animation->animationID, sizeof(UUID));

        // skeleton UUID
        in.read((char*)&animation->skeletonID, sizeof(UUID));

        // animation name
        uint32_t nameLen;
        in.read((char*)&nameLen, sizeof(uint32_t));

        animation->name.resize(nameLen);
        in.read(&animation->name[0], nameLen);

        // duration + ticks
        in.read((char*)&animation->duration, sizeof(float));
        in.read((char*)&animation->ticksPerSecond, sizeof(float));

        // track count
        uint32_t trackCount;
        in.read((char*)&trackCount, sizeof(uint32_t));

        animation->tracks.resize(trackCount);

        for (uint32_t i = 0; i < trackCount; i++)
        {
            auto& track = animation->tracks[i];

            // bone name
            uint32_t boneNameLen;
            in.read((char*)&boneNameLen, sizeof(uint32_t));

            track.boneName.resize(boneNameLen);
            in.read(&track.boneName[0], boneNameLen);

            uint32_t boneIdx;
            in.read((char*)&boneIdx, sizeof(uint32_t));
            track.boneIndex = boneIdx;

            // position keys
            uint32_t posCount;
            in.read((char*)&posCount, sizeof(uint32_t));

            track.positions.resize(posCount);
            in.read((char*)track.positions.data(), posCount * sizeof(AnimationKeyPosition));

            // rotation keys
            uint32_t rotCount;
            in.read((char*)&rotCount, sizeof(uint32_t));

            track.rotations.resize(rotCount);
            in.read((char*)track.rotations.data(), rotCount * sizeof(AnimationKeyRotation));

            // scale keys
            uint32_t scaleCount;
            in.read((char*)&scaleCount, sizeof(uint32_t));

            track.scales.resize(scaleCount);
            in.read((char*)track.scales.data(), scaleCount * sizeof(AnimationKeyScale));
        }

        return animation;
    }
}