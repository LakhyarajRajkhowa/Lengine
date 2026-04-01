#pragma once

#include "../components/Light.h"

namespace Lengine {
    class LightStorage {
    public:
        void Add(const UUID& entityID) {
            Light light = Light(entityID);
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

        const Light& Get(const UUID& entityID) const {
            return m_Lights.at(entityID);
        }

         std::unordered_map<UUID, Light>& GetAll()  {
            return m_Lights;
        }
        const std::unordered_map<UUID, Light>& GetAll() const {
            return m_Lights;
        }

        UUID& GetDirectionalShadowCasteer() {
            return directionalShadowCaster;
        }

        const UUID& GetDirectionalShadowCasteer() const {
            return directionalShadowCaster;
        }

        void SetDirectionalShadowCaster(const UUID&id)
        {
            for (auto& l : m_Lights)
            {
                if (l.second.type == LightType::Directional)
                    l.second.castShadow = false;
            }

            m_Lights[id].castShadow = true;
            directionalShadowCaster = id;
        }


        UUID& GetPointShadowCasteer() {
            return pointShadowCaster;
        }

        const UUID& GetPointShadowCasteer() const {
            return pointShadowCaster;
        }

        void SetPointShadowCaster(const UUID& id)
        {
            for (auto& l : m_Lights)
            {
                if (l.second.type == LightType::Point || l.second.type == LightType::Spotlight)
                    l.second.castShadow = false;
            }

            m_Lights[id].castShadow = true;
            pointShadowCaster = id;
        }

        void ClearDirectionalShadowCaster(const UUID& id)
        {
            if (directionalShadowCaster == id)
            {
                directionalShadowCaster = UUID::Null; // invalid UUID
                m_Lights[id].castShadow = false;
            }
        }

        void ClearPointShadowCaster(const UUID& id)
        {
            if (pointShadowCaster == id)
            {
                pointShadowCaster = UUID::Null;
                m_Lights[id].castShadow = false;
            }
        }

        

    private:
        std::unordered_map<UUID, Light> m_Lights;
        UUID directionalShadowCaster = UUID::Null;
        UUID pointShadowCaster = UUID::Null;

    };
}

