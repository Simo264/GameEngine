#include "ImGuiLayer.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

namespace ImGuiLayer
{
  void SetupContext() 
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /* Enable Keyboard Controls */
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      /* Enable Gamepad Controls */
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         /* Enable Docking */
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       /* Enable Multi-Viewport / Platform Windows */
    
    ImGui_ImplGlfw_InitForOpenGL(WindowManager::Instance()->GetContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
  }

  void CleanUp()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void SetFont(const fspath& fontpath, int fontsize)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(fontpath.string().c_str(), fontsize);
  }

  void NewFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::Begin();
  }

  void DrawData()
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

  void RenderMenuBar()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Open"))
        {
          CONSOLE_TRACE("Open...");
        }
        if (ImGui::MenuItem("Save"))
        {
          CONSOLE_TRACE("Save...");
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }

  void RenderOutlinerPanel(Scene& scene)
  {
    static bool visible = true;
    ImGui::Begin("Outliner", &visible);
    {
      if (ImGui::TreeNode("Scene"))
      {
        for (auto [entity, lComp] : scene.Reg().view<LabelComponent>().each())
        {
          ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        }

        ImGui::TreePop();
      }
    }
    ImGui::End();
  }
}