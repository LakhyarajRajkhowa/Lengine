#pragma once

#include "../scene/Scene.h"
#include "../resources/AssetManager.h"

namespace Lengine
{

    class AnimationSystem
    {
    public:
        AnimationSystem(AssetManager& assetManager) : assetManager(assetManager) {}

        void Update(Scene* scene, float dt);

    private:
        AssetManager& assetManager;

        void ApplyAnimation(
            Scene* scene,
            UUID entity,
            AnimationComponent& anim,
            float time
        );

        void ComputeBoneTransforms(
            Skeleton& skeleton,
            Animation& animation,
            float time,
            std::vector<glm::mat4>& boneMatrices
        );

        glm::vec3 InterpolatePosition(AnimationTrack& track, float time, int delta);
        glm::quat InterpolateRotation(AnimationTrack& track, float time, int delta);
        glm::vec3 InterpolateScale(AnimationTrack& track, float time, int delta);

        size_t FindKeyframeIndex(const std::vector<AnimationKeyPosition>& keys, float time);
        size_t FindKeyframeIndex(const std::vector<AnimationKeyRotation>& keys, float time);
        size_t FindKeyframeIndex(const std::vector<AnimationKeyScale>& keys, float time);




    };

}