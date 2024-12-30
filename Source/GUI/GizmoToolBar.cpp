#include "Core/Core.hpp"

#include "Core/Math/Core.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

void GUI_RenderTransformToolBar(vec2i viewportPos, i32& gizmode)
{
  auto& texManager = TextureManager::Get();
  static const auto& expandIcon = texManager.GetIconByPath(GetIconsPath() / "expand-arrows-32.png");
  static const auto& rotateIcon = texManager.GetIconByPath(GetIconsPath() / "rotate-32.png");
  static const auto& scaleIcon = texManager.GetIconByPath(GetIconsPath() / "scale-32.png");

  ImGuiStyle& style = ImGui::GetStyle();
  ImVec2 windowPos = ImVec2(viewportPos.x + 4.f, viewportPos.y + 24.f);

  constexpr ImVec2 btnSize = ImVec2(32.f, 32.f);
  const ImVec2 btnPadding = style.FramePadding; // = (4,3)
  const ImVec2 effectiveBtnSize = ImVec2(
    btnSize.x + btnPadding.x * 2,
    btnSize.y + btnPadding.y * 2); // = (40,38)
  const ImVec2 windowPadding = style.WindowPadding; // = (8,8)
  static ImVec2 windowSize = ImVec2(
    effectiveBtnSize.x + windowPadding.x * 2,
    effectiveBtnSize.y * 3 + windowPadding.y * 3);

  ImGui::SetNextWindowPos(windowPos);
  ImGui::SetNextWindowSize(windowSize);
  constexpr i32 flags = ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoCollapse;
  ImGui::Begin("Gizmo_Toolbar", nullptr, flags);

  constexpr ImVec4 btnColor = ImVec4(0.0f, 0.278f, 0.671f, 1.0f);
  constexpr ImVec4 btnColorHovered = ImVec4(0.0f, 0.248f, 0.631f, 1.0f);
  constexpr ImVec4 btnColorActive = ImVec4(0.0f, 0.228f, 0.611f, 1.0f);

  if (gizmode == ImGuizmo::OPERATION::TRANSLATE)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnColorHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, btnColorActive);
    if (ImGui::ImageButton("Gizmo_Translate", reinterpret_cast<void*>(expandIcon.id), btnSize))
      gizmode = ImGuizmo::OPERATION::TRANSLATE;
    ImGui::PopStyleColor(3);
  }
  else if (ImGui::ImageButton("Gizmo_Translate", reinterpret_cast<void*>(expandIcon.id), btnSize))
    gizmode = ImGuizmo::OPERATION::TRANSLATE;
  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Gizmo_Translate");



  if (gizmode == ImGuizmo::OPERATION::ROTATE)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnColorHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, btnColorActive);
    if (ImGui::ImageButton("Gizmo_Rotate", reinterpret_cast<void*>(rotateIcon.id), btnSize))
      gizmode = ImGuizmo::OPERATION::ROTATE;
    ImGui::PopStyleColor(3);
  }
  else if (ImGui::ImageButton("Gizmo_Rotate", reinterpret_cast<void*>(rotateIcon.id), btnSize))
    gizmode = ImGuizmo::OPERATION::ROTATE;
  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Gizmo_Rotate");

  if (gizmode == ImGuizmo::OPERATION::SCALE)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btnColorHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, btnColorActive);
    if (ImGui::ImageButton("Gizmo_Scale", reinterpret_cast<void*>(scaleIcon.id), btnSize))
      gizmode = ImGuizmo::OPERATION::SCALE;
    ImGui::PopStyleColor(3);
  }
  else if (ImGui::ImageButton("Gizmo_Scale", reinterpret_cast<void*>(scaleIcon.id), btnSize))
    gizmode = ImGuizmo::OPERATION::SCALE;
  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Gizmo_Scale");

  ImGui::End();
}
