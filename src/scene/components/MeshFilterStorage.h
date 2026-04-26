#pragma once
#include <unordered_map>
#include "MeshFilter.h"

namespace Lengine
{

    class MeshFilterStorage
    {
    public:
        MeshFilter& Add(const UUID& entityID, const MeshFilter& filter = MeshFilter())
        {
            return m_MeshFilters[entityID] = filter;
        }

        void Remove(const UUID& entityID)
        {
            m_MeshFilters.erase(entityID);
        }

        bool Has(const UUID& entityID) const
        {
            return m_MeshFilters.find(entityID) != m_MeshFilters.end();
        }

        MeshFilter& Get(const UUID& entityID)
        {
            return m_MeshFilters.at(entityID);
        }

        const MeshFilter& Get(const UUID& entityID) const
        {
            return m_MeshFilters.at(entityID);
        }

        std::unordered_map<UUID, MeshFilter>& GetAll()
        {
            return m_MeshFilters;
        }

        const std::unordered_map<UUID, MeshFilter>& GetAll() const
        {
            return m_MeshFilters;
        }

        void CloneFrom(
            const MeshFilterStorage& src,
            const std::unordered_map<UUID, UUID>& map)
        {
            for (const auto& [oldEntity, comp] : src.GetAll())
            {
                UUID newEntity = map.at(oldEntity);
                m_MeshFilters[newEntity] = comp;
            }
        }

    private:
        std::unordered_map<UUID, MeshFilter> m_MeshFilters;
    };

}
