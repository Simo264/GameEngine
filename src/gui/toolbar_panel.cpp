#include "tool_barpanel.hpp"
#include "engine/globals.hpp"
#include "engine/managers/texture_smanager.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

// Colori dei bottoni, spostati all'interno della classe
constexpr auto s_btn_color = ImVec4(0.0f, 0.278f, 0.671f, 1.0f);
constexpr auto s_btn_color_hovered = ImVec4(0.0f, 0.248f, 0.631f, 1.0f);
constexpr auto s_btn_color_active = ImVec4(0.0f, 0.228f, 0.611f, 1.0f);

ToolbarPanel::ToolbarPanel()
{
  is_open = true;

  auto& tex_manager = TexturesManager::GetInstance();
  expand_icon_ = tex_manager.GetOrCreateIcon("expand-arrows-32.png");
  scale_icon_ = tex_manager.GetOrCreateIcon("scale-32.png");
  rotate_icon_ = tex_manager.GetOrCreateIcon("rotate-32.png");
  wireframe_icon_ = tex_manager.GetOrCreateIcon("wireframe-icon-32.png");
}

void ToolbarPanel::render(std::string_view window_name, 
	                        i32 viewport_pos_x, 
	                        i32 viewport_pos_y, 
	                        i32 viewport_width, 
	                        i32 viewport_height, 
	                        i32& viewport_gizmo_op)
{
  if (!is_open)
    return;

  constexpr auto btn_spacing = 2.0f;
  constexpr auto btn_size = 32.f;

  auto toolbar_w = viewport_width * 0.3f;
  auto toolbar_h = btn_size + 22;
  auto toolbar_x = static_cast<f32>(viewport_pos_x + (viewport_width - toolbar_w) * 0.5f);
  auto toolbar_y = static_cast<f32>(viewport_pos_y + 30);

  constexpr auto flags = ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoCollapse;

  ImGui::SetNextWindowPos(ImVec2(toolbar_x, toolbar_y));
  ImGui::SetNextWindowSize(ImVec2(toolbar_w, toolbar_h));
  ImGui::SetNextWindowBgAlpha(0.75f);
  ImGui::Begin(window_name.data(), &isOpen, flags);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(btn_spacing, 0));

  // Chiamata ai metodi di classe
  gizmo_button("##ToolBar_Translate", 
               expand_icon_, 
               btn_size, 
               viewport_gizmo_op,
               static_cast<i32>(ImGuizmo::OPERATION::TRANSLATE),
               "Translate");

  ImGui::SameLine();
  
  gizmo_button("##ToolBar_Scale", 
               scale_icon_, 
               btn_size, 
               viewport_gizmo_op,
               static_cast<i32>(ImGuizmo::OPERATION::SCALE),
               "Scale");
  
  ImGui::SameLine();
  
  gizmo_button("##ToolBar_Rotate", 
                rotate_icon_, 
                btn_size, 
                viewport_gizmo_op,
                static_cast<i32>(ImGuizmo::OPERATION::ROTATE),
                "Rotate");

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(10.f, 0));
  ImGui::SameLine();

  // Qui gestiamo g_RenderInWireframe, ora parte dello stato di AppState
  bool wireframe = g_RenderInWireframe;
  gizmo_button("##ToolBar_Wireframe", 
               wireframe_icon_, 
               btn_size, 
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

void ToolbarPanel::gizmo_button(std::string_view label,
                                Texture2D icon,
                                f32 btn_size,
                                i32& viewport_gizmo_op,
                                i32 op,
                                std::string_view tooltip)
{
  auto is_active = (viewport_gizmo_op == op);
  auto current_color = is_active ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_Button];
  auto current_color_hovered = is_active ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  auto current_color_active = is_active ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, current_color);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_color_hovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, current_color_active);
  if (ImGui::ImageButton(label.data(), icon.id, ImVec2(btnSize, btnSize)))
  {
    viewport_gizmo_op = isActive ? 0 : op;
  }
  ImGui::PopStyleColor(3);

  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::Text(tooltip.data());
    ImGui::EndTooltip();
  }
}

void ToolbarPanel::wireframe_button(std::string_view label,
                                    Texture2D icon,
                                    f32 btn_size,
                                    bool& wireframe,
                                    std::string_view tooltip)
{
  auto current_color = wireframe ? btnColor : ImGui::GetStyle().Colors[ImGuiCol_Button];
  auto current_color_hovered = wireframe ? btnColorHovered : ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
  auto current_color_active = wireframe ? btnColorActive : ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_Button, current_color);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_color_hovered);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, current_color_active);
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

