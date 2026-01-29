#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>   // quat, quat_cast, toMat4
#include <glm/gtx/quaternion.hpp>   // eulerAngles, angleAxis helpers
#include <glm/gtc/matrix_transform.hpp>

#include "../scene/components/Transform.h"
#include "../utils/UUID.h"

namespace Lengine {
    class TransformSystem
    {
    public:
        static bool Dirty;

        static void RecalculateLocalMatrix(TransformComponent& t)
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, t.localPosition);
            m = glm::rotate(m, t.localRotation.x, { 1,0,0 });
            m = glm::rotate(m, t.localRotation.y, { 0,1,0 });
            m = glm::rotate(m, t.localRotation.z, { 0,0,1 });
            m = glm::scale(m, t.localScale);

            t.localMatrix = m;
            t.localDirty = false;
        }

        static void DecomposeMatrix(
            const glm::mat4& m,
            glm::vec3& position,
            glm::vec3& rotationEuler,
            glm::vec3& scale
        )
        {
            // ---- Translation ----
            position = glm::vec3(m[3]);

            // ---- Extract basis vectors ----
            glm::vec3 x = glm::vec3(m[0]);
            glm::vec3 y = glm::vec3(m[1]);
            glm::vec3 z = glm::vec3(m[2]);

            // ---- Scale ----
            scale.x = glm::length(x);
            scale.y = glm::length(y);
            scale.z = glm::length(z);

            // Prevent division by zero
            if (scale.x != 0.0f) x /= scale.x;
            if (scale.y != 0.0f) y /= scale.y;
            if (scale.z != 0.0f) z /= scale.z;

            // ---- Rotation ----
            glm::mat3 rotMat;
            rotMat[0] = x;
            rotMat[1] = y;
            rotMat[2] = z;

            glm::quat q = glm::quat_cast(rotMat);
            rotationEuler = glm::eulerAngles(q); // radians
        }
    };


}

