#pragma once
#include <string>

#include <filesystem>

namespace Lengine {
    static std::string ExtractNameFromPath(const std::string& path) {
        // Find last slash or backslash
        size_t slash = path.find_last_of("/\\");
        size_t start = (slash == std::string::npos) ? 0 : slash + 1;

        // Find last dot for extension
        size_t dot = path.find_last_of('.');
        if (dot == std::string::npos || dot < start)
            dot = path.length();

        return path.substr(start, dot - start);
    }

    static std::string ExtractNameFromPath(const std::filesystem::path& path)
    {
        return path.stem().string();
    }


    static std::string ExtractFileNameFromPath(const std::string& path) {
        // Find last slash or backslash
        size_t slash = path.find_last_of("/\\");
        size_t start = (slash == std::string::npos) ? 0 : slash + 1;



        return path.substr(start);
    }

    static std::string NormalizePath(const std::string& path)
    {
        std::string out = path;

        // Trim spaces
        out.erase(0, out.find_first_not_of(" \t"));
        out.erase(out.find_last_not_of(" \t") + 1);

        // Remove surrounding quotes if present
        if (!out.empty() && out.front() == '"' && out.back() == '"')
            out = out.substr(1, out.size() - 2);

        // Convert backslashes to forward slashes
        std::replace(out.begin(), out.end(), '\\', '/');

        return out;
    }

    static std::string StripQuotes(const std::string& s)
    {
        std::string out = s;
        out.erase(std::remove(out.begin(), out.end(), '\"'), out.end());
        out.erase(std::remove_if(out.begin(), out.end(),
            [](unsigned char c) { return std::isspace(c); }), out.end());
        return out;
    }
    static inline std::string trim(const std::string& s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    static bool IsMeshSource(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();
        return ext == ".obj" || ext == ".fbx" || ext == ".dae"
            || ext == ".gltf" || ext == ".glb";
    }


    static bool IsTextureSource(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();
        return ext == ".png" || ext == ".jpeg" || ext == ".jpg";
    }

    static bool IsMaterialSource(const std::filesystem::path& path)
    {
        return path.extension() == ".mtl";
    }

    static std::string GetFolderPath(const std::string& filepath) {
        if (filepath.empty()) return "";

        // Find the last slash (both Windows and Unix style)
        size_t pos = filepath.find_last_of("/\\");
        if (pos == std::string::npos) {
            // No slash found → file is in the current directory
            return "";
        }

        // Return substring up to (but not including) the last slash
        return filepath.substr(0, pos);
    }

    static std::string GetFileExtension(const std::string& filepath)
    {
        size_t slashPos = filepath.find_last_of("/\\");
        size_t dotPos = filepath.find_last_of('.');

        // No dot OR dot is in a directory name
        if (dotPos == std::string::npos ||
            (slashPos != std::string::npos && dotPos < slashPos))
        {
            return "";
        }

        return filepath.substr(dotPos + 1); // without '.'
    }

    static bool IsInsideDirectory(const std::filesystem::path& file, const std::filesystem::path& directory)
    {
        auto fileIt = file.begin();
        auto dirIt = directory.begin();

        for (; dirIt != directory.end(); ++dirIt, ++fileIt)
        {
            if (fileIt == file.end() || *fileIt != *dirIt)
                return false;
        }
        return true;
    }
}



