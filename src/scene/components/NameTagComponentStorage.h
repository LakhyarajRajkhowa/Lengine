#pragma once

#include <unordered_map>
#include "NameTagComponent.h"

namespace Lengine {
    class NameTagComponentStorage
    {
    public:

        void Add(UUID entityID, const NameTagComponent& tag)
        {
            components[entityID] = tag;
        }

        bool Has(UUID entityID) const
        {
            return components.find(entityID) != components.end();
        }

        NameTagComponent& Get(UUID entityID)
        {
            return components.at(entityID);
        }

        const NameTagComponent& Get(UUID entityID) const
        {
            return components.at(entityID);
        }

        void Remove(UUID entityID)
        {
            components.erase(entityID);
        }

        const std::unordered_map<UUID, NameTagComponent>& All() const {
            return components;
        }

        std::unordered_map<UUID, NameTagComponent>& All() {
            return components;
        }


        void CloneFrom(
            const NameTagComponentStorage& src,
            const std::unordered_map<UUID, UUID>& map)
        {
            for (const auto& [oldEntity, comp] : src.All())
            {
                UUID newEntity = map.at(oldEntity);
                components[newEntity] = comp;
            }
        }
    private:

        std::unordered_map<UUID, NameTagComponent> components;
    };
}