#include "Viewport.hpp"
#include "ImGuiLayer.hpp"

#include "Core/Math/Ext.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Components/Components.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

// ------------------------------------------
//                  PRIVATE                  
// ------------------------------------------

static void Viewport_GizmoWorldTranslation(Transformation& transform,
                                           const Mat4f& view, 
                                           const Mat4f& proj)
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

static void Viewport_GizmoWorldRotation(Transformation& transform,
                                        const Mat4f& view, 
                                        const Mat4f& proj)
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

static void Viewport_GizmoWorldScaling(Transformation& transform,
                                       const Mat4f& view, 
                                       const Mat4f& proj)
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


// ------------------------------------------
//                    PUBLIC                 
// ------------------------------------------

void GUI_Viewport(StringView windowName,
                  Texture2D textureImage,
                  GameObject& objSelected,
                  i32 gizmode,
                  const Mat4f& view,
                  const Mat4f& proj)
{
  ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 paddingTmp = style.WindowPadding;
  style.WindowPadding = { 0.0f, 0.0f };

  // Begin main viewport
  ImGui::Begin(windowName.data(), nullptr, ImGuiWindowFlags_NoCollapse);
  const ImVec2 winSize = ImGui::GetWindowSize();
  const ImVec2 winPos = ImGui::GetWindowPos();
  auto& guiLayer = ImGuiLayer::GetInstance();
  guiLayer.viewportSize = { winSize.x, winSize.y };
  guiLayer.viewportPos = { winPos.x, winPos.y };
  guiLayer.viewportFocused = ImGui::IsWindowFocused();

  // Begin child viewport
  ImGui::BeginChild("Viewport_Child");
  const ImVec2 winChildSize = ImGui::GetWindowSize();
  guiLayer.viewportFocused |= ImGui::IsWindowFocused();
  
  ImGui::Image(textureImage.id, winChildSize, ImVec2(0, 1), ImVec2(1, 0));
  if (objSelected.IsValid())
  {
    auto* transform = objSelected.GetComponent<Transformation>();
    if (transform)
    {
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist();
      f32 w = ImGui::GetWindowWidth();
      f32 h = ImGui::GetWindowHeight();
      ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, w, h);

      switch (gizmode)
      {
      case ImGuizmo::OPERATION::TRANSLATE:
        Viewport_GizmoWorldTranslation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::ROTATE:
        Viewport_GizmoWorldRotation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::SCALE:
        Viewport_GizmoWorldScaling(*transform, view, proj);
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
