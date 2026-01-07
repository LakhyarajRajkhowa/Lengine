#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "../utils/UUID.h"
#include "../resources/AssetManager.h"

#include "../external/tinyfiledialogs.h"
namespace Lengine {
    struct MeshDragPayload
    {
        UUID id;
        char path[512];
    };
    struct TextureDragPayload
    {
        UUID id;
        char path[512];
    };
    struct MaterialDragPayload
    {
        UUID id;
        char path[512];
    };

    class AssetPanel {
    public:
        AssetPanel(const std::filesystem::path& root, AssetManager& assetMgr);

        void OnImGuiRender();

    private:
        void DrawDirectory(const std::filesystem::path& path);
        void OpenImportMeshDialog(std::string folderPath);
        void OpenImportTextureDialog(bool srgb = false);
        void OpenImportMaterialDialog();
        void CreateNewFolder(const std::filesystem::path& path);

    private:
        AssetManager& assetManager;

        std::unordered_map<std::string, ImTextureID> thumbnailCache;
        ImTextureID LoadThumbnail(const std::string& file);

        std::filesystem::path m_RootPath;
        std::filesystem::path m_CurrentPath;
        bool  m_OpenImportMeshDialog = false;
        bool  m_OpenImportTextureDialog = false;
        bool  m_OpenImportSRGBTextureDialog = false;
        bool  m_OpenImportMaterialDialog = false;



        char m_FolderNameBuffer[256] = "";
        bool m_ShowCreateFolderPopup = false;

        bool isSelected = false;

    };

}
