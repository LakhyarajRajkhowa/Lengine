#pragma once
#include <string>

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

static bool IsMeshSource(const std::filesystem::path& path)
{
    std::string ext = path.extension().string();
    return ext == ".obj" || ext == ".fbx" || ext == ".dae"
        || ext == ".gltf" || ext == ".glb";
}


static bool IsTextureSource(const std::filesystem::path& path)
{
    return path.extension() == ".png";
}
