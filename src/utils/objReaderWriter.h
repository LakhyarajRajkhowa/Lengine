#pragma once

#include <fstream>
#include <iostream>

#include "../resources/assetDatabase/MeshAsset.h"

namespace Lengine {

    static void WriteOBJFromLSubMesh(
        const std::filesystem::path& outPath,
        const LSubMeshFile& submesh)
    {
        std::ofstream out(outPath);
        if (!out)
        {
            std::cerr << "Failed to write OBJ :" << outPath << std::endl;
            return;
        }

        out << "# Generated from .lsubmesh\n";
        out << "# SubMesh UUID: " << submesh.subMeshID.value() << "\n\n";

        // ---------------- Vertices ----------------
        for (const auto& v : submesh.vertices)
        {
            out << "v "
                << v.position.x << " "
                << v.position.y << " "
                << v.position.z << "\n";
        }
        out << "\n";

        // ---------------- Texture Coords ----------------
        bool hasUV = false;
        for (const auto& v : submesh.vertices)
        {
            if (v.texCoord != glm::vec2(0.0f))
            {
                hasUV = true;
                break;
            }
        }

        if (hasUV)
        {
            for (const auto& v : submesh.vertices)
            {
                out << "vt "
                    << v.texCoord.x << " "
                    << v.texCoord.y << "\n";
            }
            out << "\n";
        }

        // ---------------- Normals ----------------
        bool hasNormals = false;
        for (const auto& v : submesh.vertices)
        {
            if (v.normal != glm::vec3(0.0f))
            {
                hasNormals = true;
                break;
            }
        }

        if (hasNormals)
        {
            for (const auto& v : submesh.vertices)
            {
                out << "vn "
                    << v.normal.x << " "
                    << v.normal.y << " "
                    << v.normal.z << "\n";
            }
            out << "\n";
        }

        // ---------------- Faces ----------------
        // OBJ indices are 1-based
        for (size_t i = 0; i < submesh.indices.size(); i += 3)
        {
            uint32_t i0 = submesh.indices[i] + 1;
            uint32_t i1 = submesh.indices[i + 1] + 1;
            uint32_t i2 = submesh.indices[i + 2] + 1;

            out << "f ";

            if (hasUV && hasNormals)
            {
                out << i0 << "/" << i0 << "/" << i0 << " "
                    << i1 << "/" << i1 << "/" << i1 << " "
                    << i2 << "/" << i2 << "/" << i2;
            }
            else if (hasNormals)
            {
                out << i0 << "//" << i0 << " "
                    << i1 << "//" << i1 << " "
                    << i2 << "//" << i2;
            }
            else
            {
                out << i0 << " " << i1 << " " << i2;
            }

            out << "\n";
        }

        out.close();

        std::cout << "OBJ written: " << outPath << std::endl;
    }

}
