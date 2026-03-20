#pragma once

#include <unordered_map>
#include "SkeletonComponent.h"

namespace Lengine {
    class SkeletonComponentStorage
    {
    private:

        std::unordered_map<UUID, SkeletonComponent> components;

    public:

        SkeletonComponent& Add(UUID entity, const SkeletonComponent& comp = SkeletonComponent())
        {
            return components[entity] = comp;

        }

        bool Has(UUID entity)
        {
            return components.find(entity) != components.end();
        }

        SkeletonComponent& Get(UUID entity)
        {
            return components.at(entity);
        }

        const SkeletonComponent& Get(UUID entity) const
        {
            return components.at(entity);
        }


        const std::unordered_map<UUID, SkeletonComponent>& All() const {
            return components;
        }

        std::unordered_map<UUID, SkeletonComponent>& All() {
            return components;
        }

        void Remove(UUID entity)
        {
            components.erase(entity);
        }

    };
}
