#pragma once

#include <unordered_map>

#include "../utils/UUID.h"
#include "MovementComponent.h"

namespace Lengine {

    class MovementStorage
    {
    public:

        MovementComponent& Add(UUID entity, const MovementComponent& movement = MovementComponent())
        {
            return m_Data[entity] = movement;
        }

        bool Has(UUID entity) const
        {
            return m_Data.find(entity) != m_Data.end();
        }

        MovementComponent& Get(UUID entity)
        {
            return m_Data.at(entity);
        }

        const MovementComponent& Get(UUID entity) const
        {
            return m_Data.at(entity);
        }

        const std::unordered_map<UUID, MovementComponent>& All() const
        {
            return m_Data;
        }

        void Remove(UUID entity)
        {
            m_Data.erase(entity);
        }

    private:

        std::unordered_map<UUID, MovementComponent> m_Data;

    };

}