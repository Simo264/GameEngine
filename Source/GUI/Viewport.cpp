#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Core/Math/Ext.hpp"

#include "ImGuiLayer.hpp"

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
    vec3f translation{};
    vec3f scale{};
    quat  rotation{};
    vec3f skew{};
    vec4f perspective{};
    glm::decompose(model, translation, rotation, scale, skew, perspective);

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
    vec3f translation{};
    vec3f scale{};
    quat  rotation{};
    vec3f skew{};
    vec4f perspective{};
    glm::decompose(model, translation, rotation, scale, skew, perspective);

    vec3f rotationDegrees = glm::eulerAngles(rotation);  /* Get vector rotation in radians */
    rotationDegrees.x = glm::degrees(rotationDegrees.x); /* Convert it in degrees */
    rotationDegrees.y = glm::degrees(rotationDegrees.y);
    rotationDegrees.z = glm::degrees(rotationDegrees.z);
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
    vec3f translation{};
    vec3f scale{};
    quat  rotation{};
    vec3f skew{};
    vec4f perspective{};
    glm::decompose(model, translation, rotation, scale, skew, perspective);

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
  const ImVec2 winSize = ImGui::GetWindowSize();
  const ImVec2 winPos = ImGui::GetWindowPos();
  auto& guiLayer = ImGuiLayer::Get();
  guiLayer.viewportSize = { winSize.x, winSize.y };
  guiLayer.viewportPos = { winPos.x, winPos.y };
  guiLayer.viewportFocused = ImGui::IsWindowFocused();

  /* Being child viewport */
  ImGui::BeginChild("Viewport_Child");
  guiLayer.viewportFocused |= ImGui::IsWindowFocused();

  const ImVec2 winChildSize = ImGui::GetWindowSize();
  ImGui::Image(reinterpret_cast<void*>(texID), winChildSize, ImVec2(0, 1), ImVec2(1, 0));
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
