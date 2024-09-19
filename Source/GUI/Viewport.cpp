#include "Core/Core.hpp"

#include "ImGuiLayer.hpp"

#include "Core/Math/Extensions.hpp"
#include "Engine/ECS/ECS.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

/* ------------------------------------------ */
/*                  PRIVATE                   */
/* ------------------------------------------ */

static void GizmoWorldTranslation(Transform& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::TRANSLATE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    transform.position = translation;
    transform.UpdateTransformation();
  }
}
static void GizmoWorldRotation(Transform& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::ROTATE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    vec3f rotationDegrees = Math::EulerAngles(rotation);  /* Get vector rotation in radians */
    rotationDegrees.x = Math::Degrees(rotationDegrees.x); /* Convert it in degrees */
    rotationDegrees.y = Math::Degrees(rotationDegrees.y);
    rotationDegrees.z = Math::Degrees(rotationDegrees.z);
    const vec3f deltaRotation = rotationDegrees - transform.rotation;

    transform.rotation += deltaRotation;
    transform.UpdateTransformation();
  }
}
static void GizmoWorldScaling(Transform& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::SCALE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    transform.scale = scale;
    transform.UpdateTransformation();
  }
}


/* ------------------------------------------ */
/*                    PUBLIC                  */
/* ------------------------------------------ */

void GUI_RenderViewport(bool& open, u32 texID, GameObject& objSelected, i32 gizmode, const mat4f& view, const mat4f& proj)
{
  ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 paddingTmp = style.WindowPadding;
  style.WindowPadding = { 0.0f, 0.0f };

  /* Begin main viewport */
  ImGui::Begin("Viewport", &open);
  const ImVec2 viewportWinSize = ImGui::GetWindowSize();
  const ImVec2 viewportWinPos = ImGui::GetWindowPos();
  auto& guiLayer = ImGuiLayer::Get();
  guiLayer.viewportSize = { viewportWinSize.x, viewportWinSize.y };
  guiLayer.viewportPos = { viewportWinPos.x, viewportWinPos.y };
  guiLayer.viewportFocused = ImGui::IsWindowFocused();

  /* Being child viewport */
  ImGui::BeginChild("Viewport_Child");
  guiLayer.viewportFocused |= ImGui::IsWindowFocused();

  const ImVec2 viewportChildWinSize = ImGui::GetWindowSize();
  ImGui::Image(reinterpret_cast<void*>(texID), viewportChildWinSize, ImVec2(0, 1), ImVec2(1, 0));
  if (objSelected.IsValid())
  {
    auto* transform = objSelected.GetComponent<Transform>();
    if (transform)
    {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();

      f32 windowW = ImGui::GetWindowWidth();
      f32 windowH = ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowW, windowH);

      switch (gizmode)
      {
      case ImGuizmo::OPERATION::TRANSLATE:
        GizmoWorldTranslation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::ROTATE:
        GizmoWorldRotation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::SCALE:
        GizmoWorldScaling(*transform, view, proj);
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
