#pragma once
#include <unordered_map>
#include "../scene/components/MeshRenderer.h"

namespace Lengine {
    class MeshRendererStorage {
    public:
        MeshRenderer& Add(UUID e, const MeshRenderer& renderer = MeshRenderer()) {
            return m_Data[e] = renderer;
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

         std::unordered_map<UUID, MeshRenderer>& All()  {
            return m_Data;
        }

    private:
        std::unordered_map<UUID, MeshRenderer> m_Data;
    };

}
