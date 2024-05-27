//#include "ContentBrowserPanel.hpp"
//
//#include "Engine/Graphics/Texture2D.hpp"
//#include "Engine/Subsystems/TextureManager.hpp"
//
//#include <imgui/imgui.h>
//
///* -----------------------------------------------------
// *          PUBLIC METHODS
// * -----------------------------------------------------
//*/
//
//ContentBrowserPanel::ContentBrowserPanel(const char* panelName, bool visible)
//  : Panel(panelName, visible)
//{
//  _currentDir = ROOT_PATH;
//  _thumbPadding = 16.0f;
//  _thumbSize = 64.0f;
//}
//
//void ContentBrowserPanel::RenderPanel()
//{
//  ImGui::Begin(panelName.c_str(), &visible);
//  ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
//  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.25f,0.25f, 0.5f });
//  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f,0.5f,0.5f,0.5f });
//
//  Texture2D* imageIcon = nullptr;
//  if (_currentDir != ROOT_PATH)
//  {
//    imageIcon = TextureManager::Instance().GetTextureByPath(ICONS_PATH / "icon-back.png");
//    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageIcon->id), { 32,32 }))
//      _currentDir = _currentDir.parent_path();
//  }
//
//  float colSize = _thumbSize + _thumbPadding;
//  float panelWidth = ImGui::GetContentRegionAvail().x;
//  int columnCount = (int)(panelWidth / colSize);
//  if (columnCount <= 0)
//    columnCount = 1;
//
//  ImGui::Columns(columnCount, 0, false);
//  for (auto& entry : std::filesystem::directory_iterator(_currentDir))
//  {
//    fspath entryPath = entry.path();
//    string entryStr = entryPath.filename().string();
//
//    /* Ignore hidden file/directory */
//    if (entryStr[0] == '.')
//      continue;
//
//    imageIcon = GetThumbIcon(entryPath, entryStr.c_str(), entry.is_directory());
//    if (imageIcon)
//    {
//      /* Double click on thumb */
//      ImGui::ImageButton(reinterpret_cast<ImTextureID>(imageIcon->id), { _thumbSize, _thumbSize });
//      if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
//        if (entry.is_directory())
//          _currentDir /= entryStr;
//    }
//
//    ImGui::TextWrapped(entryStr.c_str());
//    ImGui::NextColumn();
//  }
//
//  ImGui::PopStyleColor(3);
//
//  ImGui::Columns(1);
//  ImGui::End();
//}
//
///* -----------------------------------------------------
// *          PRIVATE METHODS
// * -----------------------------------------------------
//*/
//
//Texture2D* ContentBrowserPanel::GetThumbIcon(const fspath& entryPath, const char* entryStr, bool isDirectory) const
//{
//  auto& instanceMT = TextureManager::Instance();
//
//  if(isDirectory)
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-folder.png");
//
//  fspath extension = entryPath.extension();
//  /* Is image */
//  if (extension == ".png" || extension == ".jpg")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-file-image.png");
//  /* Txt file */
//  else if (extension == ".txt")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-text.png");
//  /* TTf file */
//  else if (extension == ".ttf")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-font.png");
//  /* Cpp file*/
//  else if (extension == ".cpp")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-cpp.png");
//  /* Hpp file*/
//  else if (extension == ".hh" || extension == ".hpp" || extension == ".h")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-hpp.png");
//  /* Vertex shader file*/
//  else if (extension == ".vert")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-vertex.png");
//  /* Fragment shader file*/
//  else if (extension == ".frag")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-fragment.png");
//  /* Object file*/
//  else if (extension == ".obj")
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-file-obj.png");
//  /* Other file */
//  else
//    return instanceMT.GetTextureByPath(ICONS_PATH / "icon-file.png");
//    
//}
