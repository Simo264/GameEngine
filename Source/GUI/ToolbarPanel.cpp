#include "ToolbarPanel.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

// Colori dei bottoni, spostati all'interno della classe
constexpr auto btnColor = ImVec4(0.0f, 0.278f, 0.671f, 1.0f);
constexpr auto btnColorHovered = ImVec4(0.0f, 0.248f, 0.631f, 1.0f);
constexpr auto btnColorActive = ImVec4(0.0f, 0.228f, 0.611f, 1.0f);

ToolbarPanel::ToolbarPanel()
{
  isOpen = true;

  auto& texManager = TexturesManager::GetInstance();
  _expandIcon = texManager.GetOrCreateIcon("expand-arrows-32.png");
  _scaleIcon = texManager.GetOrCreateIcon("scale-32.png");
  _rotateIcon = texManager.GetOrCreateIcon("rotate-32.png");
  _wireframeIcon = texManager.GetOrCreateIcon("wireframe-icon-32.png");
}

void ToolbarPanel::Render(StringView windowName, Vec2I viewportPos, Vec2I viewportSize, i32& viewportGizmoOp)
{
  if (!isOpen)
    return;

  constexpr auto btnSpacing = 2.0f;
  constexpr auto btnSize = 32.f;

  auto toolbarW = viewportSize.x * 0.3f;
  auto toolbarH = btnSize + 22;
  auto toolbarX = static_cast<f32>(viewportPos.x + (viewportSize.x - toolbarW) * 0.5f);
  auto toolbarY = static_cast<f32>(viewportPos.y + 30);

  constexpr auto flags = ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoCollapse;

  ImGui::SetNextWindowPos(ImVec2(toolbarX, toolbarY));
  ImGui::SetNextWindowSize(ImVec2(toolbarW, toolbarH));
  ImGui::SetNextWindowBgAlpha(0.75f);
  ImGui::Begin(windowName.data(), &isOpen, flags);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(btnSpacing, 0));

  // Chiamata ai metodi di classe
  __GizmoButton("##ToolBar_Translate", 
                _expandIcon, 
                btnSize, 
                viewportGizmoOp,
                static_cast<i32>(ImGuizmo::OPERATION::TRANSLATE),
                "Translate");

  ImGui::SameLine();
  
  __GizmoButton("##ToolBar_Scale", 
                _scaleIcon, 
                btnSize, 
                viewportGizmoOp,
                static_cast<i32>(ImGuizmo::OPERATION::SCALE),
                "Scale");
  
  ImGui::SameLine();
  
  __GizmoButton("##ToolBar_Rotate", 
                _rotateIcon, 
                btnSize, 
                viewportGizmoOp,
                static_cast<i32>(ImGuizmo::OPERATION::ROTATE),
                "Rotate");

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(10.f, 0));
  ImGui::SameLine();

  // Qui gestiamo g_RenderInWireframe, ora parte dello stato di AppState
  bool wireframe = g_RenderInWireframe;
  __WireframeButton("##ToolBar_Wireframe", 
                    _wireframeIcon, 
                    btnSize, 
                    wireframe, 
                    "Wireframe");
  
  // Se il bottone è stato premuto, l'oggetto AppState che ci è stato passato è immutabile (const).
  // Per cambiare lo stato, dobbiamo passare un puntatore/riferimento non costante o usare un sistema di callback.
  // L'approccio più pulito è usare un callback o un'API per notificare il cambiamento.
  if (wireframe != g_RenderInWireframe)
  {
    // Notifica l'engine che lo stato è cambiato, ad esempio con un evento
    // AppState::GetInstance().SetRenderWireframe(wireframe);
    // O con un sistema di callback
  }

  ImGui::PopStyleVar();
  ImGui::End();
}

void ToolbarPanel::__GizmoButton(StringView label, 
                                 Texture2D icon, 
                                 f32 btnSize, 
                                 i32& gizmode, 
                                 i32 op, 
                                 StringView tooltip)
{
  auto isActive = (gizmode == op);
  auto currentColor = isActive ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_Button];
  auto currentColorHovered = isActive ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  auto currentColorActive = isActive ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, currentColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, currentColorHovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, currentColorActive);
  if (ImGui::ImageButton(label.data(), icon.id, ImVec2(btnSize, btnSize)))
  {
    gizmode = isActive ? 0 : op;
  }
  ImGui::PopStyleColor(3);

  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::Text(tooltip.data());
    ImGui::EndTooltip();
  }
}

void ToolbarPanel::__WireframeButton(StringView label,
                                     Texture2D icon,
                                     f32 btnSize,
                                     bool& wireframe,
                                     StringView tooltip)
{
  auto currentColor = wireframe ? btnColor : ImGui::GetStyle().Colors[ImGuiCol_Button];
  auto currentColorHovered = wireframe ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  auto currentColorActive = wireframe ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
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

