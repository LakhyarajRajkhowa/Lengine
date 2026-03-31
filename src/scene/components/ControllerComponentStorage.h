#pragma once

#include <unordered_map>

#include "../utils/UUID.h"
#include "ControllerComponent.h"

namespace Lengine {

    class ControllerStorage
    {
    public:

        ControllerComponent& Add(UUID entity, const ControllerComponent& controller = ControllerComponent())
        {
            return m_Data[entity] = controller;
        }

        bool Has(UUID entity) const
        {
            return m_Data.find(entity) != m_Data.end();
        }

        ControllerComponent& Get(UUID entity)
        {
            return m_Data.at(entity);
        }

        const ControllerComponent& Get(UUID entity) const
        {
            return m_Data.at(entity);
        }

        const std::unordered_map<UUID, ControllerComponent>& All() const
        {
            return m_Data;
        }

        void Remove(UUID entity)
        {
            m_Data.erase(entity);
        }

    private:

        std::unordered_map<UUID, ControllerComponent> m_Data;

    };

}