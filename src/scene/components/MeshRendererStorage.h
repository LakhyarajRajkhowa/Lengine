#pragma once
#include <unordered_map>
#include "../scene/components/MeshRenderer.h"

namespace Lengine {
    class MeshRendererStorage {
    public:
        void Add(UUID e, const MeshRenderer& mr) {
            m_Data[e] = mr;
        }

        void Remove(UUID e) {
            m_Data.erase(e);
        }

        bool Has(UUID e) const {
            return m_Data.find(e) != m_Data.end();
        }
        const MeshRenderer& Get(UUID e) const {
            return m_Data.at(e);
        }

        MeshRenderer& Get(UUID e) {
            return m_Data.at(e);
        }

        const std::unordered_map<UUID, MeshRenderer>& All() const {
            return m_Data;
        }

    private:
        std::unordered_map<UUID, MeshRenderer> m_Data;
    };

}
