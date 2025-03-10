#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include <imgui/imgui.h>

static fs::path contentBrowser_left_nodeSelected = Filesystem::GetAssetsPath();
static fs::path contentBrowser_right_navCurrentDir = contentBrowser_left_nodeSelected;

static void ContentBrowser_LeftCol_ListTree(const fs::path& currentDir)
{
  for (const auto& entry : fs::directory_iterator(currentDir))
  {
    const auto& entryPath = entry.path();
    const auto entryPathFilename = entryPath.filename().string();
    if (entry.is_directory())
    {
      i32 flags = ImGuiTreeNodeFlags_OpenOnArrow;
      if (!contentBrowser_left_nodeSelected.empty() && contentBrowser_left_nodeSelected == entryPath)
        flags |= ImGuiTreeNodeFlags_Selected;

      bool nodeOpen = ImGui::TreeNodeEx(entryPathFilename.c_str(), flags);
      
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && contentBrowser_left_nodeSelected != entryPath)
      {
        contentBrowser_left_nodeSelected = entryPath;
        contentBrowser_right_navCurrentDir = entryPath;
      }

      if (nodeOpen)
      {
        ContentBrowser_LeftCol_ListTree(entryPath);
        ImGui::TreePop();
      }
    }
    else if (entry.is_regular_file())
    {
      ImGui::TreeNodeEx(entryPathFilename.c_str(), ImGuiTreeNodeFlags_Leaf);
      ImGui::TreePop();
    }
  }
}
static void ContentBrowser_RightCol_Navbar(f32 navbarHeight)
{
  TexturesManager& texManager = TexturesManager::Get();

  ImGui::BeginChild("RightCol_Child_NavBar", ImVec2(ImGui::GetColumnWidth(), navbarHeight));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
  static Texture2D iconBack = texManager.GetOrCreateIcon("back-arrow.png");

  // Back button
  if (contentBrowser_right_navCurrentDir == Filesystem::GetAssetsPath())
  {
    ImGui::BeginDisabled();
    ImGui::ImageButton("Back", reinterpret_cast<void*>(iconBack.id), ImVec2(16.0f, 16.0f));
    ImGui::EndDisabled();
  }
  else if (ImGui::ImageButton("Back", reinterpret_cast<void*>(iconBack.id), ImVec2(16.0f, 16.0f)))
    contentBrowser_right_navCurrentDir = contentBrowser_right_navCurrentDir.parent_path();

  ImGui::SameLine();
  ImGui::TextWrapped(contentBrowser_right_navCurrentDir.string().c_str());
  ImGui::PopStyleColor();
  ImGui::EndChild();
  
  // Draw the border-bottom
  auto& colors = ImGui::GetStyle().Colors;
  auto color = colors[ImGuiCol_TableBorderLight];
  ImGui::PushStyleColor(ImGuiCol_Separator, color);
  ImGui::Separator();
  ImGui::PopStyleColor();
}
static void ContentBrowser_RightCol_Thumbnails(f32 contentHeight)
{
  ImGui::BeginChild("RightCol_Child_Thumbnails", ImVec2(ImGui::GetColumnWidth(), contentHeight));
  if (!contentBrowser_right_navCurrentDir.empty())
  {
    constexpr i32 cellSize = 64.0f;
    constexpr i32 cellPadding = 16.0f;
    const f32 panelWidth = ImGui::GetContentRegionAvail().x;
    const i32 nColumns = std::max(1, static_cast<i32>(panelWidth / (cellSize + cellPadding)));
    if (ImGui::BeginTable("Thumbnails", nColumns, ImGuiTableFlags_SizingStretchSame))
    {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
      
      fs::directory_iterator entryIt = fs::directory_iterator(contentBrowser_right_navCurrentDir);
      const i32 nItemsInCurrentDir = std::distance(fs::begin(entryIt), fs::end(entryIt));
      const i32 nRows = 1 + (nItemsInCurrentDir / nColumns);
      entryIt = fs::directory_iterator(contentBrowser_right_navCurrentDir);
      for (i32 i = 0; i < nRows; i++)
      {
        ImGui::TableNextRow();
        for (i32 j = 0; j < nColumns && entryIt != fs::end(entryIt); j++, entryIt++)
        {
          ImGui::TableSetColumnIndex(j);

          const fs::directory_entry& entry = *entryIt;
          const fs::path& absolutePath = entry.path();
          const fs::path relativePath = fs::relative(absolutePath, Filesystem::GetIconsPath());
          const fs::path filename = relativePath.filename();
          const String filenameString = filename.string();
          auto& manager = TexturesManager::Get();

          if (entry.is_directory())
          {
            static Texture2D iconFolder = manager.GetOrCreateIcon("open-folder.png");
            if (ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(iconFolder.id), ImVec2(cellSize, cellSize)))
              contentBrowser_right_navCurrentDir /= entry;
          }
          else
          {
            const fs::path ext = filename.extension();

            // Render image texture
            if (ext == ".png" || ext == ".jpg")
            {
              Texture2D texture{};
              if (contentBrowser_right_navCurrentDir == Filesystem::GetIconsPath())
                texture = manager.FindIcon(relativePath);
              else
                texture = manager.FindTexture(relativePath);

              ImGui::ImageButton(filenameString.c_str(), 
                reinterpret_cast<void*>(texture.id), 
                ImVec2(cellSize, cellSize)
              );
            }
            // Render generic file icon
            else
            {
              static Texture2D iconFile = manager.GetOrCreateIcon("file.png");
              
              ImGui::ImageButton(filenameString.c_str(), 
                reinterpret_cast<void*>(iconFile.id), 
                ImVec2(cellSize, cellSize)
              );
            }
          }

          if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip("%s", absolutePath.relative_path().string().c_str());
          ImGui::TextWrapped("%s", filenameString.c_str());
        }
      }
      ImGui::PopStyleColor();
      ImGui::EndTable();
    }
  }
  ImGui::EndChild();
}

// ------------------------------------------
//                    PUBLIC                 
// ------------------------------------------

void GUI_RenderContentBrowser(bool& open)
{
  ImGui::Begin("Content browser", &open);

  f32 tableHeight = ImGui::GetContentRegionAvail().y - 10.f;
  constexpr i32 flags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInner;
  if (ImGui::BeginTable("SplitTable", 2, flags))
  {
    ImGui::TableSetupColumn("LeftCol", ImGuiTableColumnFlags_WidthFixed, 250.0f);
    ImGui::TableSetupColumn("RightCol", ImGuiTableColumnFlags_WidthStretch);

    // Left column 
    ImGui::TableNextColumn();
    ImGui::BeginChild("LeftCol_Child", ImVec2(ImGui::GetColumnWidth(), tableHeight));
    {
      const auto& assetsDir = Filesystem::GetAssetsPath();
      i32 flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
      if (contentBrowser_left_nodeSelected == assetsDir)
        flags |= ImGuiTreeNodeFlags_Selected;

      if (ImGui::TreeNodeEx(assetsDir.filename().string().c_str(), flags))
      {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && contentBrowser_left_nodeSelected != assetsDir)
        {
          contentBrowser_left_nodeSelected = assetsDir;
          contentBrowser_right_navCurrentDir = assetsDir;
        }
        ContentBrowser_LeftCol_ListTree(assetsDir);
        ImGui::TreePop();
      }
    }
    ImGui::EndChild();
    // End column



    /* Right column */
    ImGui::TableNextColumn();
    ImGui::BeginChild("RightCol_Child", ImVec2(ImGui::GetColumnWidth(), tableHeight));
    {
      static constexpr f32 navbarHeight = 25.f;
      ContentBrowser_RightCol_Navbar(navbarHeight);

      f32 contentHeight = tableHeight - navbarHeight - 10.f;
      ContentBrowser_RightCol_Thumbnails(contentHeight);
    }
    ImGui::EndChild();
    /* End column */


    ImGui::EndTable();
  }
  ImGui::End();
}
