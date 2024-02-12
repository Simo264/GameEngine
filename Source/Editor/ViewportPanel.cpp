#include "ViewportPanel.hh"
#include "../FrameBuffer.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

ViewportPanel::ViewportPanel(const char* panelName, Vec2i viewportSize)
{
  this->viewportSize = viewportSize;
  this->panelName = panelName;
  
  isOpen = true;
	isFocused = false;
}

void ViewportPanel::RenderPanel(FrameBuffer* framebuffer)
{
  /* Set viewport window padding to 0 */
  ImGuiStyle& style = ImGui::GetStyle();
  Vec2i paddingTmp = Vec2i(style.WindowPadding.x, style.WindowPadding.y);
  style.WindowPadding.x = 0;
  style.WindowPadding.y = 0;

  ImGui::Begin(panelName.c_str(), &isOpen);
  isFocused = ImGui::IsWindowFocused();

  /* Using a Child allow to fill all the space of the window. It also alows customization */
  ImGui::BeginChild("GameRender");
  isFocused = isFocused || ImGui::IsWindowFocused();

  /* Get the size of the child(i.e.the whole draw size of the windows). */
  ImVec2 drawSpace = ImGui::GetWindowSize();
  Vec2i framebufferSize = framebuffer->GetSize();
  if (viewportSize.x != drawSpace.x || viewportSize.y != drawSpace.y)
  {
    viewportSize = Vec2i(drawSpace.x, drawSpace.y);
    framebuffer->RescaleFrameBuffer(viewportSize);
    glViewport(0, 0, viewportSize.x, viewportSize.y);
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
