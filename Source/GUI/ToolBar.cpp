#include "ToolBar.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Components/GameObject.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include <imgui.h>
#include <ImGuizmo.h>




constexpr ImVec4 btnColor = ImVec4(0.0f, 0.278f, 0.671f, 1.0f);
constexpr ImVec4 btnColorHovered = ImVec4(0.0f, 0.248f, 0.631f, 1.0f);
constexpr ImVec4 btnColorActive = ImVec4(0.0f, 0.228f, 0.611f, 1.0f);

// ----------------------------------------------------
//          PRIVATE
// ----------------------------------------------------

static void ToolBar_GizmoButton(StringView label,
                                Texture2D icon,
                                f32 btnSize,
                                i32& gizmode,
                                ImGuizmo::OPERATION op,
                                StringView tooltip)
{
  bool isActive = (gizmode == op);
  ImVec4 currentColor = isActive ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_Button];
  ImVec4 currentColorHovered = isActive ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  ImVec4 currentColorActive = isActive ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentColorHovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentColorActive);
  if (ImGui::ImageButton(label.data(), icon.id, ImVec2(btnSize, btnSize)))
    gizmode = isActive ? 0 : op;
  ImGui::PopStyleColor(3);
  
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::Text(tooltip.data());
    ImGui::EndTooltip();
  }
}

static void ToolBar_WireframeButton(StringView label, 
                                    Texture2D icon,
                                    f32 btnSize,
                                    bool& wireframe,
                                    StringView tooltip)
{
  ImVec4 currentColor = wireframe ? btnColor : ImGui::GetStyle().Colors[ImGuiCol_Button];
  ImVec4 currentColorHovered = wireframe ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  ImVec4 currentColorActive = wireframe ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentColorHovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentColorActive);
  if (ImGui::ImageButton(label.data(), icon.id, ImVec2(btnSize, btnSize)))
    wireframe = !wireframe;
  ImGui::PopStyleColor(3);

  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::Text(tooltip.data());
    ImGui::EndTooltip();
  }
}

static void ToolBar_NormalButton(StringView label, 
                                 Texture2D icon, 
                                 f32 btnSize, 
                                 bool& normalMap,
                                 StringView tooltip)
{
  ImVec4 currentColor = normalMap ? btnColor : ImGui::GetStyle().Colors[ImGuiCol_Button];
  ImVec4 currentColorHovered = normalMap ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  ImVec4 currentColorActive = normalMap ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentColorHovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentColorActive);
  if (ImGui::ImageButton(label.data(), icon.id, ImVec2(btnSize, btnSize)))
    normalMap = !normalMap;
  ImGui::PopStyleColor(3);

  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::Text(tooltip.data());
    ImGui::EndTooltip();
  }
}

// ----------------------------------------------------
//          PUBLIC
// ----------------------------------------------------

void GUI_ToolBar(bool& open, 
                 Vec2I viewportPos, 
                 Vec2I viewportSize,
                 i32 &gizmode)
{
  constexpr f32 btnSpacing = 2.0f;
  constexpr f32 btnSize = 32.f;
  f32 toolbarW = viewportSize.x * 0.3f;
  f32 toolbarH = btnSize + 22;
  f32 toolbarX = viewportPos.x + (viewportSize.x - toolbarW) * 0.5f;
  f32 toolbarY = viewportPos.y + 30;

  constexpr i32 flags = ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoCollapse;
  ImGui::SetNextWindowPos(ImVec2(toolbarX, toolbarY));
  ImGui::SetNextWindowSize(ImVec2(toolbarW, toolbarH));
  ImGui::SetNextWindowBgAlpha(0.75f);
  ImGui::Begin("Toolbar", &open, flags);
  
  auto& texManager = TexturesManager::GetInstance();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(btnSpacing, 0));
  static Texture2D expandIcon = texManager.GetOrCreateIcon("expand-arrows-32.png");
  ToolBar_GizmoButton("ToolBar_Translate", expandIcon, btnSize, gizmode, ImGuizmo::OPERATION::TRANSLATE, "Translate");

  ImGui::SameLine();
  static Texture2D scaleIcon = texManager.GetOrCreateIcon("scale-32.png");
  ToolBar_GizmoButton("ToolBar_Scale", scaleIcon, btnSize, gizmode, ImGuizmo::OPERATION::SCALE, "Scale");
  
  ImGui::SameLine();
  static Texture2D rotateIcon = texManager.GetOrCreateIcon("rotate-32.png");
  ToolBar_GizmoButton("ToolBar_Rotate", rotateIcon, btnSize, gizmode, ImGuizmo::OPERATION::ROTATE, "Rotate");

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(10.f, 0));
  ImGui::SameLine();
  
  static Texture2D wireframeIcon = texManager.GetOrCreateIcon("wireframe-icon-32.png");
  ToolBar_WireframeButton("ToolBar_Wireframe", wireframeIcon, btnSize, g_renderInWireframe, "Wireframe");
  static Texture2D textileIcon = texManager.GetOrCreateIcon("textile-icon-32.png");
  ImGui::SameLine();
  ToolBar_WireframeButton("ToolBar_Normal", textileIcon, btnSize, g_renderWithNormalMapping, "Normal map");

  ImGui::PopStyleVar();
  ImGui::End();
}
