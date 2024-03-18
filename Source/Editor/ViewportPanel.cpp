#include "ViewportPanel.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Math/Extensions.hpp"

#include "Engine/Camera.hpp"

#include "Engine/ECS/GameObject.hpp"
#include "Engine/ECS/Components.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ViewportPanel::ViewportPanel(const char* panelName, bool visible)
  : Panel(panelName, visible),
    isFocused{ false },
    viewportSize{ 0, 0 }, /* Set size on Render() function */
    _grizmoMode{ static_cast<int>(ImGuizmo::OPERATION::TRANSLATE) } 
{}

void ViewportPanel::RenderPanel(Camera& camera, const uint32_t framebufferImage, GameObject& selected)
{
  /* Set viewport padding */
  ImGuiStyle& style = ImGui::GetStyle();
  const Vec2i paddingTmp = { style.WindowPadding.x, style.WindowPadding.y };
  style.WindowPadding.x = 0;
  style.WindowPadding.y = 0;

  ImGui::Begin(panelName.c_str(), &visible);
  isFocused = ImGui::IsWindowFocused();

  /* Using a child allow to fill all the space of the window. It also alows customization */
  ImGui::BeginChild("GameRender");
  isFocused = isFocused || ImGui::IsWindowFocused();

  /* Get the whole draw size of the window */
  const ImVec2 viewport = ImGui::GetWindowSize();
  viewportSize = { viewport.x, viewport.y };

  ImGui::Image(reinterpret_cast<ImTextureID>(framebufferImage), viewport, ImVec2(0, 1), ImVec2(1, 0));
  if (selected.IsValid())
  {
    auto component = selected.GetComponent<TransformComponent>();
    if(component)
      GrizmoTransformation(camera, *component);
  }

  ImGui::EndChild();
  ImGui::End();

  /* Restore padding */
  style.WindowPadding.x = paddingTmp.x;
  style.WindowPadding.y = paddingTmp.y;
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ViewportPanel::GrizmoTransformation(Camera& camera, TransformComponent& component)
{
  Mat4f& model = component.GetTransformation();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();

  float windowW = ImGui::GetWindowWidth();
  float windowH = ImGui::GetWindowHeight();
  ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowW, windowH);

  auto& view = camera.cameraComponent->GetView();
  auto& projection = camera.cameraComponent->GetProjection();
  ImGuizmo::Manipulate(
    &view[0][0],
    &projection[0][0],
    static_cast<ImGuizmo::OPERATION>(_grizmoMode), 
    ImGuizmo::WORLD, 
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    static Vec3f translation;
    static Vec3f scale;
    static Quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    Vec3f rotationDegrees = Math::EulerAngles(rotation);  /* Get vector rotation in radians */
    rotationDegrees.x = Math::Degrees(rotationDegrees.x); /* Convert it in degrees */
    rotationDegrees.y = Math::Degrees(rotationDegrees.y);
    rotationDegrees.z = Math::Degrees(rotationDegrees.z);
    const Vec3f deltaRotation = rotationDegrees - component.rotation;
    
    component.position = translation;
    component.scale = scale;
    component.rotation += deltaRotation;
  }
}
