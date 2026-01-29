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

    private:
        std::unordered_map<UUID, HierarchyComponent> m_Data;
    };

}
