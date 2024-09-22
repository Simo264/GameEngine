#include "Core/Core.hpp"
#include "ImGuiLayer.hpp"
#include "Engine/Subsystems/FontManager.hpp"

#include <imgui/imgui.h>

void GUI_RenderPreferencesWindow(bool& open, i32 fontSize)
{
  ImGui::Begin("Preferences", &open);
  static constexpr const char* leftItemList[] = {
    "Font",
    "...",
  };
  static i32 selected = 0;

  /* Left panel */
  {
    ImGui::BeginChild("Left", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
    for (i32 i = 0; i < std::size(leftItemList); i++)
    {
      if (ImGui::Selectable(leftItemList[i], selected == i))
        selected = i;
    }
    ImGui::EndChild();
  }
  /* End panel */

  ImGui::SameLine();

  /* Right panel */
  {
    ImGuiLayer& guiLayer = ImGuiLayer::Get();

    ImGui::BeginGroup();
    ImGui::BeginChild("Item_View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    switch (selected)
    {
    case 0: /* Font properties */
      ImGui::SeparatorText("Font properties");
      ImGui::TextWrapped("Font size: %dpx", fontSize);
      if (ImGui::BeginCombo("Font family", guiLayer.selectedFont.first->c_str()))
      {
        for (const auto& [key, font] : FontManager::Get().GetFonts())
        {
          if (ImGui::Selectable(key.c_str(), guiLayer.selectedFont.first == &key) && guiLayer.selectedFont.first != &key)
          {
            guiLayer.selectedFont = std::make_pair(const_cast<String*>(&key), const_cast<fs::path*>(&font));
            guiLayer.changeFontFamily = true;
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
  /* End panel */

  ImGui::End();
}
