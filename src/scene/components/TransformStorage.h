#pragma once

#include <vector>
#include <unordered_map>
#include "../utils/UUID.h"
#include "Transform.h"

namespace Lengine {

    class TransformStorage {
    public:

        TransformComponent& Add(UUID entity, const TransformComponent& t = {})
        {
            size_t index = m_Components.size();

            m_Entities.push_back(entity);
            m_Components.push_back(t);
            m_Lookup[entity] = index;

            return m_Components.back();
        }

        bool Has(UUID entity) const
        {
            return m_Lookup.find(entity) != m_Lookup.end();
        }

        TransformComponent& Get(UUID entity)
        {
            return m_Components[m_Lookup.at(entity)];
        }

        const TransformComponent& Get(UUID entity) const
        {
            return m_Components[m_Lookup.at(entity)];
        }

        void Remove(UUID entity)
        {
            size_t index = m_Lookup.at(entity);
            size_t lastIndex = m_Components.size() - 1;

            // swap with last
            m_Components[index] = m_Components[lastIndex];
            m_Entities[index] = m_Entities[lastIndex];

            // update moved entity lookup
            m_Lookup[m_Entities[index]] = index;

            // remove last
            m_Components.pop_back();
            m_Entities.pop_back();
            m_Lookup.erase(entity);
        }

        std::vector<TransformComponent>& All()
        {
            return m_Components;
        }

        const std::vector<TransformComponent>& All() const
        {
            return m_Components;
        }

        std::vector<UUID>& Entities()
        {
            return m_Entities;
        }

    private:
        std::vector<TransformComponent> m_Components; // dense
        std::vector<UUID> m_Entities;                  // dense
        std::unordered_map<UUID, size_t> m_Lookup;     // sparse
    };

}