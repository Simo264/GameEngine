#include "ViewportPanel.hh"
#include "../FrameBuffer.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

ViewportPanel::ViewportPanel(const char* name, Vec2i size)
{
	isOpen = true;
	isFocused = false;
	viewportSize = size;
  vewportPadding = Vec2i(0, 0);
	panelName = name;
}

void ViewportPanel::RenderPanel(FrameBuffer* framebuffer)
{
  if (!isOpen) 
    return;

  /* Set viewport window padding to 0 */
  ImGuiStyle& style = ImGui::GetStyle();
  Vec2i paddingTmp = Vec2i(style.WindowPadding.x, style.WindowPadding.y);
  style.WindowPadding.x = vewportPadding.x;
  style.WindowPadding.y = vewportPadding.y;

  ImGui::SetNextWindowSize(ImVec2(viewportSize.x, viewportSize.y));
  ImGui::Begin(panelName.c_str(), &isOpen);
  isFocused = ImGui::IsWindowFocused();

  /* Using a Child allow to fill all the space of the window. It also alows customization */
  ImGui::BeginChild("GameRender");
  isFocused = isFocused || ImGui::IsWindowFocused();

  /* Get the size of the child(i.e.the whole draw size of the windows). */
  ImVec2 drawSpace = ImGui::GetWindowSize();
  Vec2i framebufferSize = framebuffer->GetSize();
  if ((drawSpace.x != framebufferSize.x || drawSpace.y != framebufferSize.y))
  {
    framebuffer->RescaleFrameBuffer({ drawSpace.x, drawSpace.y });
    glViewport(0, 0, drawSpace.x, drawSpace.y);
    viewportSize = Vec2i(drawSpace.x, drawSpace.y);
  }
  else
  {
    /* Because I use the texture from OpenGL, I need to invert the V from the UV. */
    ImGui::Image((ImTextureID)framebuffer->GetImage(), drawSpace, ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::EndChild();
  ImGui::End();

  /* Restore padding */
  style.WindowPadding.x = paddingTmp.x;
  style.WindowPadding.y = paddingTmp.y;
}
