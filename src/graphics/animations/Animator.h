#pragma once

#include "../animations/Animation.h"
#include "../animations/Skeleton.h"

namespace Lengine {
    static void BindAnimationToSkeleton(Animation& animation, const Skeleton& skeleton)
    {
        for (auto& track : animation.tracks)
        {
            auto it = skeleton.boneMap.find(track.boneName);

            if (it != skeleton.boneMap.end())
                track.boneIndex = it->second;
            else
                track.boneIndex = -1;
        }
    }


}
