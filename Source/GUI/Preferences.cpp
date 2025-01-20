#include "ImGuiLayer.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Globals.hpp"

#include <imgui/imgui.h>

static Vector<fs::path> fontVector;

static void LoadFontVector()
{
  fontVector.clear();
  for (const auto& entry : fs::recursive_directory_iterator(Filesystem::GetFontsPath())) 
  {
    if (!fs::is_regular_file(entry.path()))
      continue;

    fs::path relativePath = fs::relative(entry.path(), Filesystem::GetFontsPath());
    fontVector.emplace_back(relativePath);
  }
}


void GUI_RenderPreferencesWindow(bool& open)
{
  ImGui::Begin("Preferences", &open);
  static constexpr const char* leftItemList[] = {
    "Font",
    "...",
  };
  static i32 selected = 0;

  // Left panel
  {
    ImGui::BeginChild("Left", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
    for (i32 i = 0; i < std::size(leftItemList); i++)
    {
      if (ImGui::Selectable(leftItemList[i], selected == i))
        selected = i;
    }
    ImGui::EndChild();
  }
  // End panel 

  ImGui::SameLine();

  // Right panel
  {
    ImGuiLayer& guiLayer = ImGuiLayer::Get();

    ImGui::BeginGroup();
    ImGui::BeginChild("Item_View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    switch (selected)
    {
    case 0: // Font properties
      ImGui::SeparatorText("Font properties");
      ImGui::TextWrapped("Current font: %s", g_fontFamily.string().c_str());
      
      LoadFontVector();

      if (ImGui::BeginCombo("Font family", g_fontFamily.string().c_str()))
      {
        for (const auto& font : fontVector)
        {
          if (ImGui::Selectable(font.string().c_str(), font == g_fontFamily))
          {
            g_fontFamily = font;
            guiLayer.changeFontFamilyFlag = true;
          }
        }
        ImGui::EndCombo();
      }
      break;

    default:
      ImGui::SeparatorText("Todo");
      break;
    }
    ImGui::EndChild();
    ImGui::EndGroup();
  }
  // End panel 

  ImGui::End();
}
