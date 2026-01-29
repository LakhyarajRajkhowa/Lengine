#pragma once
#include <unordered_map>
#include "../utils/UUID.h"

namespace Lengine
{

    struct MeshFilter
    {

        UUID submeshID = UUID::Null;

        // async / deferred loading support
        bool pendingSubmesh = false;
        UUID pendingSubmeshID = UUID::Null;

        void RequestSubmesh(const UUID& id)
        {
            pendingSubmeshID = id;
            pendingSubmesh = true;
        }

        bool HasPendingSubmesh() const
        {
            return pendingSubmesh;
        }

        UUID GetRequestedSubmeshID() const
        {
            return pendingSubmeshID;
        }

        void ClearPendingSubmesh()
        {
            pendingSubmesh = false;
        }
    };

}
