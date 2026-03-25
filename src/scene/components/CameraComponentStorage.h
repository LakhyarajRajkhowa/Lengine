#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include "CameraComponent.h"
#include "../utils/UUID.h"

namespace Lengine {

    class CameraComponentStorage
    {
    private:

        std::unordered_map<UUID, CameraComponent> cameras;

        UUID primaryCamera = UUID::Null;

    public:

        // Add camera
        CameraComponent& Add(const UUID& entityID)
        {
            cameras[entityID] = CameraComponent();

            if (primaryCamera == UUID::Null)
                primaryCamera = entityID;

            return cameras[entityID];
        }

        // Remove camera
        void Remove(const UUID& entityID)
        {
            cameras.erase(entityID);

            if (primaryCamera == entityID)
                primaryCamera = UUID::Null;
        }

        // Check if entity has camera
        bool Has(const UUID& entityID) const
        {
            return cameras.find(entityID) != cameras.end();
        }

        // Get camera
        CameraComponent& Get(const UUID& entityID)
        {
            return cameras.at(entityID);
        }

        const CameraComponent& Get(const UUID& entityID) const
        {
            return cameras.at(entityID);
        }

        // Set primary camera
        void SetPrimaryCamera(const UUID& entityID)
        {
            if (Has(entityID) || entityID == UUID::Null)
                primaryCamera = entityID;
        }

        // Get primary camera
        CameraComponent* GetPrimaryCamera()
        {
            if (primaryCamera == UUID::Null)
                return nullptr;

            return &cameras[primaryCamera];
        }

        UUID GetPrimaryCameraID() const
        {
            return primaryCamera;
        }

        // Get all cameras
        const std::unordered_map<UUID, CameraComponent>& GetAllCameras() const
        {
            return cameras;
        }

    };
}
