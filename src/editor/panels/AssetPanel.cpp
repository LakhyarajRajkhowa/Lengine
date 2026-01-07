#include "AssetPanel.h"
#include <imgui.h>

namespace fs = std::filesystem;

using namespace Lengine;

AssetPanel::AssetPanel(const fs::path& root, AssetManager& asstMgr)
    : m_RootPath(root), m_CurrentPath(root),
    assetManager(asstMgr)
{

}


void AssetPanel::OnImGuiRender()
{
    ImGui::Begin("Assets");

    if (m_CurrentPath != m_RootPath)
    {
        if (ImGui::Button("<"))
        {
            m_CurrentPath = m_CurrentPath.parent_path();
        }
        ImGui::SameLine();
    }

    ImGui::Text("Current: %s", m_CurrentPath.string().c_str());
    ImGui::Separator();

    // Create Folder
    if (ImGui::Button("Create Folder"))
    {
        m_ShowCreateFolderPopup = true;
        strcpy(m_FolderNameBuffer, "");
        ImGui::OpenPopup("CreateFolderPopup");
    }

    if (ImGui::BeginPopup("CreateFolderPopup"))
    {
        ImGui::InputText("Folder Name", m_FolderNameBuffer, sizeof(m_FolderNameBuffer));

        if (ImGui::Button("Create"))
        {
            CreateNewFolder(m_CurrentPath / m_FolderNameBuffer);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Directory
    DrawDirectory(m_CurrentPath);

    ImGui::End();
}

void AssetPanel::DrawDirectory(const fs::path& path)
{
    const float thumbnailSize = 64.0f;
    const float padding = 12.0f;
    const float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, nullptr, false);

    for (auto& entry : fs::directory_iterator(path))
    {
        fs::path file = entry.path();
        std::string name = file.filename().string();

        ImGui::PushID(name.c_str());

        // ============================================================
        //  FOLDER
        // ============================================================
        if (entry.is_directory())
        {
            ImTextureID folderIcon = LoadThumbnail(Paths::Icons + "folder_icon.png");

            // --- Selectable hitbox wrapping thumbnail + text ---
            ImGui::BeginGroup();

            ImGui::Image(folderIcon, { thumbnailSize, thumbnailSize });
            bool clicked = ImGui::IsItemClicked();

            ImGui::TextWrapped("%s", name.c_str());

            ImGui::EndGroup();

            if (clicked)
                m_CurrentPath /= name;

            ImGui::NextColumn();
            ImGui::PopID();
            continue;
        }

        // ============================================================
        // ONLY .meta FILES (ALL ASSETS)
        // ============================================================
        if (file.extension() != ".meta")
        {
            ImGui::PopID();
            continue;
        }

        std::string actualFile = file.replace_extension("").string();
        MetaFile meta = MetaFileSystem::Load(actualFile);

        if (meta.uuid.value() == 0)
        {
            ImGui::PopID();
            continue;
        }

        fs::path sourcePath = meta.source;


        ImTextureID icon = 0;

        if (IsMeshSource(sourcePath))
        {
            std::string previewImage = actualFile + ".png";
            icon = fs::exists(previewImage)
                ? LoadThumbnail(previewImage)
                : LoadThumbnail(Paths::Icons + "mesh_icon.png");
        }
        else if (IsTextureSource(sourcePath))
        {
            icon = LoadThumbnail(Paths::Icons + "texture_icon.png");
        }
        else if (IsMaterialSource(sourcePath))
        {
            icon = LoadThumbnail(Paths::Icons + "material_icon.png");
        }
        else
        {
            ImGui::PopID();
            continue;
        }


        // SELECTABLE GROUP (TODO : make thumbnail selectable)
        ImGui::BeginGroup();

        ImGui::Image(icon, { thumbnailSize, thumbnailSize });
        bool clicked = ImGui::IsItemClicked();

        // Add a selectable for the name 
        ImGui::Selectable(ExtractNameFromPath(name).c_str(), false);

        if (clicked)
            ImGui::SetItemDefaultFocus();

        // ============================================================
        // DRAG AND DROP
        // ============================================================
        if (ImGui::BeginDragDropSource())
        {
            if (IsMeshSource(sourcePath))
            {
                MeshDragPayload payload;
                payload.id = meta.uuid;

                strncpy(payload.path, meta.source.c_str(), sizeof(payload.path));
                payload.path[sizeof(payload.path) - 1] = '\0';

                ImGui::SetDragDropPayload("MESH_ASSET", &payload, sizeof(payload));
            }
            else if (IsTextureSource(sourcePath))
            {
                TextureDragPayload payload;
                payload.id = meta.uuid;

                strncpy(payload.path, meta.source.c_str(), sizeof(payload.path));
                payload.path[sizeof(payload.path) - 1] = '\0';

                ImGui::SetDragDropPayload("TEXTURE_ASSET", &payload, sizeof(payload));
            }
            else if (IsMaterialSource(sourcePath))
            {
                MaterialDragPayload payload;
                payload.id = meta.uuid;

                strncpy(payload.path, meta.source.c_str(), sizeof(payload.path));
                payload.path[sizeof(payload.path) - 1] = '\0';

                ImGui::SetDragDropPayload("MATERIAL_ASSET", &payload, sizeof(payload));
            }

            ImGui::Text("%s", ExtractNameFromPath(name).c_str());
            ImGui::EndDragDropSource();
        }


        ImGui::EndGroup();
        ImGui::NextColumn();
        ImGui::PopID();
    }

    // RIGHT-CLICK CONTEXT MENU 
  
    if (ImGui::BeginPopupContextWindow("DirContextMenu", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Import Mesh..."))
            m_OpenImportMeshDialog = true;

        if (ImGui::MenuItem("Import Material..."))
            m_OpenImportMaterialDialog = true;

        if (ImGui::MenuItem("Import Texture..."))
            m_OpenImportTextureDialog = true;

        if (ImGui::MenuItem("Import SRGB Texture..."))
            m_OpenImportSRGBTextureDialog = true;

        ImGui::EndPopup();
    }
    

    ImGui::Columns(1);

    if (m_OpenImportMeshDialog)
    {
        m_OpenImportMeshDialog = false;
        OpenImportMeshDialog(m_CurrentPath.string());
    }
    else if (m_OpenImportTextureDialog) {
        m_OpenImportTextureDialog = false;
        OpenImportTextureDialog();
    }
    else if (m_OpenImportSRGBTextureDialog) {
        m_OpenImportSRGBTextureDialog = false;
        OpenImportTextureDialog(true);
    }
    else if (m_OpenImportMaterialDialog) {
        m_OpenImportMaterialDialog = false;
        OpenImportMaterialDialog();
    }
}



void AssetPanel::CreateNewFolder(const fs::path& path)
{
    try
    {
        if (!fs::exists(path))
            fs::create_directory(path);
    }
    catch (...) {}
}


void AssetPanel::OpenImportMeshDialog(std::string folderPath)
{
    const char* filters[6] = { "*.obj", "*.fbx", "*.dae", "*.gltf", "*.glb", "*.blend" };

    const char* filePath = tinyfd_openFileDialog(
        "Import Mesh",
        "",
        6,
        filters,
        "Mesh Files",
        0
    );

    if (filePath)
    {
        assetManager.importMesh(filePath);
    }
}
void AssetPanel::OpenImportTextureDialog(bool srgb)
{
    const char* filters[3] = { "*.png", "*.jpeg", "*.jpg"};

    const char* filePath = tinyfd_openFileDialog(
        "Import Texture",
        "",
        3,
        filters,
        "Texture Files",
        0
    );

    if (filePath)
    {
        assetManager.importTexture(filePath, srgb);
    }
}
void AssetPanel::OpenImportMaterialDialog()
{
    const char* filters[1] = { "*.mtl" };

    const char* filePath = tinyfd_openFileDialog(
        "Import Material",
        "",
        1,
        filters,
        "Material Files",
        0
    );

    if (filePath)
    {
        assetManager.importMaterial(filePath);
    }
}


ImTextureID AssetPanel::LoadThumbnail(const std::string& file)
{
    if (thumbnailCache.count(file))
        return thumbnailCache[file];

    GLTexture* tex = assetManager.loadImage(file, file);
    if (!tex)
        return -1;

    ImTextureID id = (ImTextureID)(intptr_t)tex->id;
    thumbnailCache[file] = id;

    return id;
}
