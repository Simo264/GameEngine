#include "ViewportPanel.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

extern Mat4f cameraProjectionMatrix;
extern Mat4f cameraViewMatrix;

void ViewportPanel::RenderPanel(FrameBuffer* framebuffer)
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
  Vec2i framebufferSize = framebuffer->GetSize();
  viewportSize = { viewport.x, viewport.y };

  if (framebufferSize.x != viewportSize.x || framebufferSize.y != viewportSize.y)
  {
    framebuffer->RescaleFrameBuffer(viewportSize.x, viewportSize.y);
    glViewport(0, 0, viewportSize.x, viewportSize.y);
  }
  else
  {
    ImGui::Image((ImTextureID)framebuffer->GetImage(), viewport, ImVec2(0, 1), ImVec2(1, 0));
    
    //StaticMesh* mesh = static_cast<StaticMesh*>(objSelected);
    //if (mesh)
    //{
    //  ImGuizmo::SetOrthographic(false);
    //  ImGuizmo::SetDrawlist();

    //  Window window(glfwGetCurrentContext());
    //  Vec2f windowSize = window.GetWindowSize();
    //  ImGuizmo::SetRect(mesh->transform.position.x, mesh->transform.position.y, windowSize.x, windowSize.y);

    //  Mat4f transform = mesh->transform.GetTransformation();

    //  ImGuizmo::Manipulate(&cameraViewMatrix[0][0], &cameraProjectionMatrix[0][0], 
    //    ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, &transform[0][0]);
    //}
  }
  ImGui::EndChild();
  ImGui::End();

  /* Restore padding */
  style.WindowPadding.x = paddingTmp.x;
  style.WindowPadding.y = paddingTmp.y;
}
