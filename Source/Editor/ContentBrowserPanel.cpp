#include "ContentBrowserPanel.hpp"

#include "Engine/Texture2D.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Logger.hpp"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ContentBrowserPanel::ContentBrowserPanel(const char* panelName)
{
  this->panelName = panelName;
	isOpen = true;
  
  _currentDir = ROOT_PATH;
  _thumbPadding = 16.0f;
  _thumbSize = 64.0f;
}

void ContentBrowserPanel::RenderPanel()
{
  ImGui::Begin(panelName.c_str(), &isOpen);
  ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.25f,0.25f, 0.5f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f,0.5f,0.5f,0.5f });

  Texture2D* imageIcon = nullptr;
  if (_currentDir != ROOT_PATH)
  {
    imageIcon = TexturesManager::Instance().GetTextureByPath(ROOT_PATH / "Icons/icon-back.png");
    if (ImGui::ImageButton((ImTextureID)imageIcon->textureID, { 32,32 }))
      _currentDir = _currentDir.parent_path();
  }

  float colSize = _thumbSize + _thumbPadding;
  float panelWidth = ImGui::GetContentRegionAvail().x;
  int columnCount = (int)(panelWidth / colSize);
  if (columnCount <= 0)
    columnCount = 1;

  ImGui::Columns(columnCount, 0, false);
  for (auto& entry : std::filesystem::directory_iterator(_currentDir))
  {
    Path entryPath = entry.path();
    String entryStr = entryPath.filename().string();

    /* Ignore hidden file/directory */
    if (entryStr[0] == '.')
      continue;

    imageIcon = GetThumbIcon(entryPath, entryStr.c_str(), entry.is_directory());

    /* Double click on thumb */
    ImGui::ImageButton((ImTextureID)imageIcon->textureID, { _thumbSize, _thumbSize });
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
      if (entry.is_directory())
        _currentDir /= entryStr;

    ImGui::TextWrapped(entryStr.c_str());
    ImGui::NextColumn();
  }

  ImGui::PopStyleColor(3);

  ImGui::Columns(1);
  ImGui::End();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

Texture2D* ContentBrowserPanel::GetThumbIcon(const Path& entryPath, const char* entryStr, bool isDirectory) const
{
  auto& instanceMT = TexturesManager::Instance();

  if(isDirectory)
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-folder.png");

  Path extension = entryPath.extension();
  /* Is image */
  if (extension == ".png" || extension == ".jpg")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-file-image.png");
  /* Txt file */
  else if (extension == ".txt")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-text.png");
  /* TTf file */
  else if (extension == ".ttf")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-font.png");
  /* Cpp file*/
  else if (extension == ".cpp")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-cpp.png");
  /* Hpp file*/
  else if (extension == ".hh" || extension == ".hpp" || extension == ".h")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-hpp.png");
  /* Vertex shader file*/
  else if (extension == ".vert")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-vertex.png");
  /* Fragment shader file*/
  else if (extension == ".frag")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-fragment.png");
  /* Object file*/
  else if (extension == ".obj")
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-file-obj.png");
  /* Other file */
  else
    return instanceMT.GetTextureByPath(ROOT_PATH / "Icons/icon-file.png");
    
}
