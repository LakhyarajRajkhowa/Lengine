#pragma once

#include <unordered_map>

#include "Transform.h"
#include "../utils/UUID.h"

namespace Lengine {
    class TransformStorage
    {
    public:
        TransformComponent& Add(UUID entity, const TransformComponent& transform = TransformComponent())
        {
            return m_Data[entity] = transform;
        }

        bool Has(UUID entity) const
        {
            return m_Data.find(entity) != m_Data.end();
        }

        TransformComponent& Get(UUID entity)
        {
       
            return m_Data.at(entity);
        }

        const TransformComponent& Get(UUID entity) const
        {
            return m_Data.at(entity);
        }

        const std::unordered_map<UUID, TransformComponent>& All() const
        {
            return m_Data;
        }

        void Remove(UUID e) {
            m_Data.erase(e);
        }

    private:
        std::unordered_map<UUID, TransformComponent> m_Data;
    };


}

