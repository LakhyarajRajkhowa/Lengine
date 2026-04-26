#pragma once

#include <unordered_map>

#include "Hierarchy.h"
#include "../utils/UUID.h"

namespace Lengine {

    class HierarchyStorage
    {
    public:
        HierarchyComponent& Add(UUID entity, const HierarchyComponent& h = HierarchyComponent())
        {
            return m_Data[entity] = h;
        }

        bool Has(UUID entity) const
        {
            return m_Data.find(entity) != m_Data.end();
        }

        HierarchyComponent& Get(UUID entity)
        {
            return m_Data.at(entity);
        }

        const HierarchyComponent& Get(UUID entity) const
        {
            return m_Data.at(entity);
        }

        void Remove(UUID entity)
        {
            m_Data.erase(entity);
        }

        const std::unordered_map<UUID, HierarchyComponent>& All() const
        {
            return m_Data;
        }

        void CloneFrom(
            const HierarchyStorage& src,
            const std::unordered_map<UUID, UUID>& entityMap)
        {
            m_Data.clear();

            for (const auto& [oldEntity, oldComp] : src.All())
            {
                // Get new entity ID
                auto it = entityMap.find(oldEntity);
                if (it == entityMap.end())
                    continue;

                UUID newEntity = it->second;

                HierarchyComponent newComp;

                // 🔹 Remap parent
                if (oldComp.parent != UUID::Null)
                {
                    auto parentIt = entityMap.find(oldComp.parent);
                    newComp.parent = (parentIt != entityMap.end())
                        ? parentIt->second
                        : UUID::Null;
                }

                // 🔹 Remap children
                newComp.children.reserve(oldComp.children.size());

                for (const UUID& child : oldComp.children)
                {
                    auto childIt = entityMap.find(child);
                    if (childIt != entityMap.end())
                    {
                        newComp.children.push_back(childIt->second);
                    }
                }

                // 🔹 Store
                m_Data[newEntity] = std::move(newComp);
            }
        }
    private:
        std::unordered_map<UUID, HierarchyComponent> m_Data;
    };

}
