#pragma once

#include "../components/Light.h"

namespace Lengine {
    class LightStorage {
    public:
        void Add(const UUID& entityID, const Light& light = Light()) {
            m_Lights[entityID] = light;
        }

        void Remove(const UUID& entityID) {
            m_Lights.erase(entityID);
        }

        bool Has(const UUID& entityID) const {
            return m_Lights.find(entityID) != m_Lights.end();
        }

        Light& Get(const UUID& entityID) {
            
            return m_Lights.at(entityID);
        }

        const Light& Get(const UUID& entityID) const{
            return m_Lights.at(entityID);
        }

        const std::unordered_map<UUID, Light>& GetAll() const {
            return m_Lights;
        }

    private:
        std::unordered_map<UUID, Light> m_Lights;
    };
}

