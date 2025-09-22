#include "ViewportPanel.hpp"
#include "ImGuiLayer.hpp"

#include "Engine/Scene.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

void ViewportPanel::Render(StringView windowName,
                           Texture2D viewportImage,
                           Entity entityTarget,
                           const Mat4F& view,
                           const Mat4F& proj)
{
  if (!isOpen)
    return;

  auto& style = ImGui::GetStyle();
  auto paddingTmp = style.WindowPadding;
  style.WindowPadding = { 0.0f, 0.0f };

  ImGui::Begin(windowName.data(), &isOpen, ImGuiWindowFlags_NoCollapse);
  auto winSize = ImGui::GetWindowSize();
  auto winPos = ImGui::GetWindowPos();
  auto& guiLayer = ImGuiLayer::GetInstance();
  size = { winSize.x, winSize.y };
  position = { winPos.x, winPos.y };
  isFocused = ImGui::IsWindowFocused();

  // Begin child viewport
  ImGui::BeginChild("Viewport_Child");
  auto winChildSize = ImGui::GetWindowSize();
  isFocused |= ImGui::IsWindowFocused();

  ImGui::Image(viewportImage.id, winChildSize, ImVec2(0, 1), ImVec2(1, 0));
  if (entityTarget.IsValid())
  {
    auto transform = entityTarget.GetComponent<Components::Transform>();
    if (transform != nullptr)
    {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();
      auto w = ImGui::GetWindowWidth();
      auto h = ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, w, h);

      switch (gizmoOp)
      {
        case ImGuizmo::OPERATION::TRANSLATE:
          __GizmoWorldTranslation(*transform);
          break;

        case ImGuizmo::OPERATION::ROTATE:
          __GizmoWorldRotation(*transform);
          break;

        case ImGuizmo::OPERATION::SCALE:
          __GizmoWorldScaling(*transform);
          break;

        default:
          break;
      }
    }
  }
  ImGui::EndChild();
  ImGui::End();
  style.WindowPadding = paddingTmp;
}



void ViewportPanel::__GizmoWorldTranslation(Components::Transform& transform)
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
void ViewportPanel::__GizmoWorldRotation(Components::Transform& transform)
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

  //  vec3f rotationDegrees = glm::eulerAngles(rotation);   /* Get vector rotation in radians */
  //  rotationDegrees = glm::degrees(rotationDegrees);      /* Convert it in degrees */
  //  const vec3f deltaRotation = rotationDegrees - transform.GetEulerAnglesVector();

  //  transform.Rotate(transform.GetEulerAnglesVector() + deltaRotation);
  //}
}
void ViewportPanel::__GizmoWorldScaling(Components::Transform& transform)
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
