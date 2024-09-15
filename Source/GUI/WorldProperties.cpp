#include "Core/Core.hpp"
#include "Engine/Globals.hpp"

#include <imgui/imgui.h>

void GUI_RenderWorldProperties(bool& open)
{
  ImGui::Begin("World", &open);

  if (ImGui::CollapsingHeader("Ambient"))
  {
    ImGui::ColorEdit3("Light color", (f32*)&g_ambientColor);
    ImGui::SliderFloat("Light intensity", &g_ambientIntensity, 0.0f, 1.0f);
  }

  ImGui::End();
}
