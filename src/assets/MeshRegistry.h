#pragma once
#include "../utils/UUID.h"

namespace Lengine {

    struct SubmeshID
    {
        static inline const UUID Plane = UUID(16770060616967427546);
        static inline const UUID Sphere = UUID(9216755572843866765);

        static const std::vector<UUID>& GetAllDefaults()
        {
            static const std::vector<UUID> defaults = {
                Sphere,
                Plane
            };
            return defaults;
        }
    };

}
