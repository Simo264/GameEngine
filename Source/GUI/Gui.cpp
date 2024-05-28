#include "Gui.hpp"

#include "Core/Log/Logger.hpp"

#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Gui::SetupContext() const
{
	/* Init ImGui */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       /* Enable Multi-Viewport / Platform Windows */

  /* Setup Platform/Renderer backends */
  ImGui_ImplGlfw_InitForOpenGL(WindowManager::Instance()->GetContext(), true);
  
  /* OpenGL version 4.6 */
  ImGui_ImplOpenGL3_Init("#version 460");
}

void Gui::CleanUp() const
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Gui::StartFrame() const
{
  /* Start the Dear ImGui frame */
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::Begin();
}

void Gui::EndFrame() const
{
  auto window = WindowManager::Instance();

  ImGui::RenderPanel();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    WindowManager::Context backup_current_context = window->GetContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    window->SetContext(backup_current_context);
  }
}

void Gui::Render() const
{
}
