#include "ViewportPanel.hpp"

#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

#include "Engine/ECS/GameObject.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/Systems.hpp"

#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

extern Mat4f cameraProjectionMatrix;
extern Mat4f cameraViewMatrix;

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

void ViewportPanel::RenderPanel(FrameBuffer& framebuffer, GameObject* selected)
{
  /* Set viewport window padding to 0 */
  ImGuiStyle& style = ImGui::GetStyle();
  Vec2i paddingTmp = Vec2i(style.WindowPadding.x, style.WindowPadding.y);
  style.WindowPadding.x = 0;
  style.WindowPadding.y = 0;

  ImGui::Begin(panelName.c_str(), &visible);
  isFocused = ImGui::IsWindowFocused();

  /* Using a Child allow to fill all the space of the window. It also alows customization */
  ImGui::BeginChild("GameRender");
  isFocused = isFocused || ImGui::IsWindowFocused();

  /* Get the size of the child(i.e.the whole draw size of the windows). */
  ImVec2 viewport = ImGui::GetWindowSize();
  Vec2i framebufferSize = framebuffer.GetSize();
  viewportSize = { viewport.x, viewport.y };

  if (framebufferSize.x != viewportSize.x || framebufferSize.y != viewportSize.y)
  {
    framebuffer.RescaleFrameBuffer(viewportSize.x, viewportSize.y);
    glViewport(0, 0, viewportSize.x, viewportSize.y);
  }
  else
  {
    ImGui::Image(reinterpret_cast<ImTextureID>(framebuffer.GetImage()), viewport, ImVec2(0, 1), ImVec2(1, 0));
    if (selected != nullptr)
    {
      auto component = selected->GetComponent<TransformComponent>();
      if(component)
        GrizmoTransformation(*component);
    }
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

void ViewportPanel::GrizmoTransformation(TransformComponent& component)
{
  Mat4f model = component.GetTransformation();

  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();

  float windowW = ImGui::GetWindowWidth();
  float windowH = ImGui::GetWindowHeight();
  ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowW, windowH);

  ImGuizmo::Manipulate(&cameraViewMatrix[0][0], &cameraProjectionMatrix[0][0],
    static_cast<ImGuizmo::OPERATION>(_grizmoMode), ImGuizmo::WORLD, &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    static Vec3f translation;
    static Vec3f scale;
    static Quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    const Vec3f deltaRotation = Math::EulerAngles(rotation) - component.rotation;
    UpdatePosition(component, translation);
    UpdateScale(component, scale);
    UpdateRotation(component, component.rotation + deltaRotation);
  }
}
