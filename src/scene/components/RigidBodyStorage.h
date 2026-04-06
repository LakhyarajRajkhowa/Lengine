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

    private:

        std::vector<RigidbodyComponent> m_Components;
        std::vector<UUID> m_Entities;
        std::unordered_map<UUID, size_t> m_Lookup;
    };

}