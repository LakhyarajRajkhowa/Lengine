#pragma once

#include "AnimatonComponent.h"

namespace Lengine
{
    class AnimationComponentStorage
    {
    public:

        AnimationComponent& Add(UUID entity, const AnimationComponent& component = AnimationComponent())
        {
            auto it = entityToIndex.find(entity);
            if (it != entityToIndex.end())
                return components[it->second];

            uint32_t index = (uint32_t)components.size();

            components.push_back(component);
            entities.push_back(entity);

            entityToIndex[entity] = index;

            return components.back();
        }

        void Remove(UUID entity)
        {
            auto it = entityToIndex.find(entity);
            if (it == entityToIndex.end())
                return;

            uint32_t index = it->second;
            uint32_t lastIndex = (uint32_t)components.size() - 1;

            components[index] = components[lastIndex];
            entities[index] = entities[lastIndex];

            entityToIndex[entities[index]] = index;

            components.pop_back();
            entities.pop_back();

            entityToIndex.erase(entity);
        }

        AnimationComponent* Get(UUID entity)
        {
            auto it = entityToIndex.find(entity);
            if (it == entityToIndex.end())
                return nullptr;

            return &components[it->second];
        }

        const AnimationComponent* Get(UUID entity) const
        {
            auto it = entityToIndex.find(entity);
            if (it == entityToIndex.end())
                return nullptr;

            return &components[it->second];
        }

        std::vector<AnimationComponent>& GetAll()
        {
            return components;
        }

        const std::vector<AnimationComponent>& GetAll() const 
        {
            return components;
        }

        const std::vector<UUID>& GetEntities() const
        {
            return entities;
        }

        bool Has(UUID entity) const
        {
            return entityToIndex.find(entity) != entityToIndex.end();
        }
    private:

        std::vector<AnimationComponent> components;
        std::vector<UUID> entities;

        std::unordered_map<UUID, uint32_t> entityToIndex;
    };
}