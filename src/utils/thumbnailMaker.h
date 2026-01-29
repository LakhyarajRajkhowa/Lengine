#pragma once

#include <vector>
#include <filesystem>

#include "../external/stb_image_write.h"

#include "../graphics/geometry/Bounds.h"


namespace Lengine {
    static Bounds ComputeBounds(const std::vector<Vertex>& vertices)
    {
        Bounds b;
        b.aabb.reset();

        for (const auto& v : vertices)
            b.aabb.expand(v.position);

        b.center = b.aabb.center();

        // Bounding sphere radius (cheap)
        float maxDistSq = 0.0f;

        for (const auto& v : vertices)
        {
            glm::vec3 d = v.position - b.center;
            maxDistSq = glm::max(maxDistSq, glm::dot(d, d));
        }

        b.radius = std::sqrt(maxDistSq);

        return b;
    }

    inline glm::vec2 IsoProject(const glm::vec3& p)
    {
        return {
            p.x - p.z * 0.6f,
            p.y + p.z * 0.3f
        };
    }


    static void WriteBoundsThumbnail(
        const Bounds& bounds,
        const std::filesystem::path& outPath,
        int size = 64
    )
    {
        std::vector<uint8_t> pixels(size * size * 4, 255);

        auto drawPixel = [&](int x, int y)
            {
                if (x < 0 || y < 0 || x >= size || y >= size) return;
                int idx = (y * size + x) * 4;
                pixels[idx + 0] = 120;
                pixels[idx + 1] = 190;
                pixels[idx + 2] = 255;
                pixels[idx + 3] = 255;
            };

        // Normalize using radius (important!)
        float scale = 1.0f / bounds.radius;

        glm::vec3 corners[8] = {
            {bounds.aabb.min.x, bounds.aabb.min.y, bounds.aabb.min.z},
            {bounds.aabb.max.x, bounds.aabb.min.y, bounds.aabb.min.z},
            {bounds.aabb.max.x, bounds.aabb.max.y, bounds.aabb.min.z},
            {bounds.aabb.min.x, bounds.aabb.max.y, bounds.aabb.min.z},
            {bounds.aabb.min.x, bounds.aabb.min.y, bounds.aabb.max.z},
            {bounds.aabb.max.x, bounds.aabb.min.y, bounds.aabb.max.z},
            {bounds.aabb.max.x, bounds.aabb.max.y, bounds.aabb.max.z},
            {bounds.aabb.min.x, bounds.aabb.max.y, bounds.aabb.max.z}
        };

        for (auto& c : corners)
        {
            glm::vec3 local = (c - bounds.center) * scale;
            glm::vec2 p = IsoProject(local);

            int x = int((p.x + 1.0f) * 0.5f * size);
            int y = int((1.0f - (p.y + 1.0f) * 0.5f) * size);

            drawPixel(x, y);
        }

        stbi_write_png(
            outPath.string().c_str(),
            size,
            size,
            4,
            pixels.data(),
            size * 4
        );
    }

}
