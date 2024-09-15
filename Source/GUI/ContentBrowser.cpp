#include "Core/Core.hpp"

#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>

void GUI_RenderContentBrowser(bool& open)
{
  ImGui::Begin("Content browser", &open);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

  TextureManager& texManager = TextureManager::Get();

  /* Back button */
  static fs::path currentDir = GetResourcePath();
  if (currentDir != GetResourcePath())
  {
    const auto& iconBack = texManager.GetIconByPath(GetIconsPath() / "back-arrow.png");
    if (ImGui::ImageButton("Back", reinterpret_cast<void*>(iconBack.id), ImVec2(16.0f, 16.0f)))
      currentDir = currentDir.parent_path();
  }

  /* Configure table */
  constexpr f32 cellSize = 64.0f;
  constexpr f32 cellPadding = 16.0f;
  const f32 panelWidth = ImGui::GetContentRegionAvail().x;
  const i32 nColumns = std::max(1, static_cast<i32>(panelWidth / (cellSize + cellPadding)));
  if (ImGui::BeginTable("Thumbnails", nColumns, ImGuiTableFlags_SizingStretchSame))
  {
    fs::directory_iterator entryIt = fs::directory_iterator(currentDir);
    const i32 nItemsInCurrentDir = std::distance(fs::begin(entryIt), fs::end(entryIt));
    const i32 nRows = 1 + (nItemsInCurrentDir / nColumns);
    entryIt = fs::directory_iterator(currentDir);

    for (i32 i = 0; i < nRows; i++)
    {
      ImGui::TableNextRow();
      for (i32 j = 0; j < nColumns && entryIt != fs::end(entryIt); j++, entryIt++)
      {
        ImGui::TableSetColumnIndex(j);

        const fs::directory_entry& entry = *entryIt;
        const fs::path& entryPath = entry.path();
        const String entryPathString = entryPath.string();
        const fs::path filename = entryPath.filename();
        const String filenameString = filename.string();

        if (entry.is_directory())
        {
          Texture2D& iconFolder = texManager.GetIconByPath(GetIconsPath() / "open-folder.png");
          if (ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(iconFolder.id), ImVec2(cellSize, cellSize)))
            currentDir /= entry;
        }
        else
        {
          const fs::path entryFilenameExt = filename.extension();

          /* Render image texture */
          if (entryFilenameExt == ".png" || entryFilenameExt == ".jpg")
          {
            Texture2D* texture = nullptr;
            if (currentDir == GetIconsPath())
              texture = &texManager.GetIconByPath(entryPathString);
            else
              texture = &texManager.GetTextureByPath(entryPathString);

            ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(texture->id), ImVec2(cellSize, cellSize));
          }
          /* Render generic file icon */
          else
          {
            const auto& iconFile = texManager.GetIconByPath(GetIconsPath() / "file.png");
            ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(iconFile.id), ImVec2(cellSize, cellSize));
          }
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
          ImGui::SetTooltip("%s", entryPathString.c_str());
        ImGui::TextWrapped("%s", filenameString.c_str());
      }
    }
    ImGui::EndTable();
  }

  ImGui::PopStyleColor();
  ImGui::End();
}
