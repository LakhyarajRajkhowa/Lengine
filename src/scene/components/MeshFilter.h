#pragma once
#include <unordered_map>
#include "../utils/UUID.h"

namespace Lengine
{

    struct MeshFilter
    {
        UUID meshID = UUID::Null;

        // async / deferred loading support
        bool pendingMesh = false;
        UUID pendingMeshID = UUID::Null;

        void RequestMesh(const UUID& id)
        {
            pendingMeshID = id;
            pendingMesh = true;
        }

        bool HasPendingMesh() const
        {
            return pendingMesh;
        }

        UUID GetRequestedMeshID() const
        {
            return pendingMeshID;
        }

        void ClearPendingMesh()
        {
            pendingMesh = false;
        }
    };

}
