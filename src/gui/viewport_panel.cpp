#include "viewport_panel.hpp"
#include "imgui_layer.hpp"
#include "engine/ecs/scene.hpp"

#include "core/math/vector.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

void ViewportPanel::render(std::string_view window_name,
                           Texture2D viewport_image,
                           Entity target,
                           const mat4f& view,
                           const mat4f& proj)
{
  if (!is_open || !_scene)
    return;

  auto& style = ImGui::GetStyle();
  auto padding_tmp = style.WindowPadding;
  style.WindowPadding = { 0.0f, 0.0f };

  ImGui::Begin(window_name.data(), &is_open, ImGuiWindowFlags_NoCollapse);
  auto win_size = ImGui::GetWindowSize();
  auto win_pos = ImGui::GetWindowPos();
  auto& gui_layer = ImGuiLayer::GetInstance();
  width = win_size.x;
  height = win_size.y;
  pos_x = win_pos.x;
  pos_y = win_pos.y;
  is_focused = ImGui::IsWindowFocused();

  // Begin child viewport
  ImGui::BeginChild("Viewport_Child");
  auto win_child_size = ImGui::GetWindowSize();
  is_focused |= ImGui::IsWindowFocused();

  ImGui::Image(viewport_image.id, win_child_size, ImVec2(0, 1), ImVec2(1, 0));
  if (target.Valid())
  {
    auto transform = _scene->TryGetEntityComponent<Components::Transform>(target);
    if (transform != nullptr)
    {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();
      auto w = ImGui::GetWindowWidth();
      auto h = ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, w, h);

      switch (gizmo_op)
      {
        case ImGuizmo::OPERATION::TRANSLATE:
          gizmo_world_translation(*transform);
          break;

        case ImGuizmo::OPERATION::ROTATE:
          gizmo_world_rotation(*transform);
          break;

        case ImGuizmo::OPERATION::SCALE:
          gizmo_world_scaling(*transform);
          break;

        default:
          break;
      }
    }
  }
  ImGui::EndChild();
  ImGui::End();
  style.WindowPadding = padding_tmp;
}

// ==============================
// Private methods
// ==============================

void ViewportPanel::gizmo_world_translation(Components::Transform& transform)
{
  //mat4f model = transform.GetTransformation();
  //ImGuizmo::Manipulate(
  //  &view[0][0],
  //  &proj[0][0],
  //  ImGuizmo::OPERATION::TRANSLATE,
  //  ImGuizmo::WORLD,
  //  &model[0][0]);

  //if (ImGuizmo::IsUsing())
  //{
  //  vec3f translation{};
  //  vec3f scale{};
  //  quat  rotation{};
  //  vec3f skew{};
  //  vec4f perspective{};
  //  glm::decompose(model, translation, rotation, scale, skew, perspective);

  //  transform.Translate(translation);
  //}
}
void ViewportPanel::gizmo_world_rotation(Components::Transform& transform)
{
  //mat4f model = transform.GetTransformation();
  //ImGuizmo::Manipulate(
  //  &view[0][0],
  //  &proj[0][0],
  //  ImGuizmo::OPERATION::ROTATE,
  //  ImGuizmo::WORLD,
  //  &model[0][0]);

  //if (ImGuizmo::IsUsing())
  //{
  //  vec3f translation{};
  //  vec3f scale{};
  //  quat  rotation{};
  //  vec3f skew{};
  //  vec4f perspective{};
  //  glm::decompose(model, translation, rotation, scale, skew, perspective);

  //  vec3f rotation_degrees = glm::eulerAngles(rotation);   /* Get vector rotation in radians */
  //  rotation_degrees = glm::degrees(rotation_degrees);      /* Convert it in degrees */
  //  const vec3f delta_rotation = rotation_degrees - transform.GetEulerAnglesVector();

  //  transform.Rotate(transform.GetEulerAnglesVector() + delta_rotation);
  //}
}
void ViewportPanel::gizmo_world_scaling(Components::Transform& transform)
{
  //mat4f model = transform.GetTransformation();
  //ImGuizmo::Manipulate(
  //  &view[0][0],
  //  &proj[0][0],
  //  ImGuizmo::OPERATION::SCALE,
  //  ImGuizmo::WORLD,
  //  &model[0][0]);

  //if (ImGuizmo::IsUsing())
  //{
  //  vec3f translation{};
  //  vec3f scale{};
  //  quat  rotation{};
  //  vec3f skew{};
  //  vec4f perspective{};
  //  glm::decompose(model, translation, rotation, scale, skew, perspective);

  //  transform.Scale(scale);
  //}
}