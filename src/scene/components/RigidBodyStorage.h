#pragma once

#include <vector>
#include <unordered_map>
#include "utils/UUID.h"
#include "RigidbodyComponent.h"

namespace Lengine {

    class RigidbodyStorage
    {
    public:

        RigidbodyComponent& Add(UUID entity, const RigidbodyComponent& rb = {})
        {
            size_t index = m_Components.size();

            m_Entities.push_back(entity);
            m_Components.push_back(rb);
            m_Lookup[entity] = index;

            return m_Components.back();
        }

        bool Has(UUID entity) const
        {
            return m_Lookup.find(entity) != m_Lookup.end();
        }

        RigidbodyComponent& Get(UUID entity)
        {
            return m_Components[m_Lookup.at(entity)];
        }

        const RigidbodyComponent& Get(UUID entity) const
        {
            return m_Components[m_Lookup.at(entity)];
        }

        void Remove(UUID entity)
        {
            size_t index = m_Lookup.at(entity);
            size_t lastIndex = m_Components.size() - 1;

            // swap
            m_Components[index] = m_Components[lastIndex];
            m_Entities[index] = m_Entities[lastIndex];

            m_Lookup[m_Entities[index]] = index;

            m_Components.pop_back();
            m_Entities.pop_back();
            m_Lookup.erase(entity);
        }

        std::vector<RigidbodyComponent>& All()
        {
            return m_Components;
        }

        const std::vector<RigidbodyComponent>& All() const
        {
            return m_Components;
        }

        std::vector<UUID>& Entities()
        {
            return m_Entities;
        }

        void CloneFrom(
            const RigidbodyStorage& src,
            const std::unordered_map<UUID, UUID>& entityMap)
        {
            m_Components.clear();
            m_Entities.clear();
            m_Lookup.clear();

            m_Components.reserve(src.m_Components.size());
            m_Entities.reserve(src.m_Entities.size());

            for (size_t i = 0; i < src.m_Components.size(); i++)
            {
                UUID oldEntity = src.m_Entities[i];

                auto it = entityMap.find(oldEntity);
                if (it == entityMap.end())
                    continue;

                UUID newEntity = it->second;

                size_t newIndex = m_Components.size();

                m_Components.push_back(src.m_Components[i]);
                m_Entities.push_back(newEntity);
                m_Lookup[newEntity] = newIndex;
            }
        }

    private:

        std::vector<RigidbodyComponent> m_Components;
        std::vector<UUID> m_Entities;
        std::unordered_map<UUID, size_t> m_Lookup;
    };

}