#pragma once
#include "../utils/UUID.h"

namespace Lengine {

    struct SubmeshID
    {
        static inline const UUID Plane = UUID(9601935706876165333);
        static inline const UUID Sphere = UUID(10506254062053502876);
        static inline const UUID Cube = UUID(16424944298018889870);
        static inline const UUID Arrow = UUID(17280307292566253951);

        static const std::vector<UUID>& GetAllDefaults()
        {
            static const std::vector<UUID> defaults = {
                Plane,
                Sphere,
                Cube,
                Arrow
            };
            return defaults;
        }
    };

}
