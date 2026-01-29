#pragma once
#include "../utils/UUID.h"

namespace Lengine {

    struct MaterialID
    {
        static inline const UUID DefaultPbr = UUID(421013474112522599);

        static const std::vector<UUID>& GetAllDefaults()
        {
            static const std::vector<UUID> defaults = {
               DefaultPbr
            };
            return defaults;
        }

    };

}
