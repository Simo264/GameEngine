#include "MenuBar.hpp"
#include "ImGuiLayer.hpp"

#include "Core/FileDialog.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <imgui.h>

void MenuBar::Render(Scene& scene)
{
  if (ImGui::BeginMainMenuBar())
  {
    __RenderFileMenu(scene);
    __RenderViewMenu();
    ImGui::EndMainMenuBar();
  }
}

void MenuBar::__RenderFileMenu(Scene& scene)
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Open"))
    {
      const char* filterPatterns[] = { "*.yaml" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto filePath = FileDialog::OpenFileDialog("Open new scene",
                                                 Utils::GetRootPath(),
                                                 nrFilterPatterns,
                                                 filterPatterns,
                                                 "Scene file (*yaml)",
                                                 false);
      if (!filePath.empty())
      {
        scene.Clear();
        scene.LoadFromFile(filePath);
      }
    }

    if (ImGui::MenuItem("Save as..."))
    {
      const char* filterPatterns[] = { "*.yaml" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto path = FileDialog::SaveFileDialog("Save scene",
                                             Utils::GetRootPath(),
                                             nrFilterPatterns,
                                             filterPatterns,
                                             "Scene file (*yaml)");
      if (!path.empty())
        scene.SaveToFile(path);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
    {
      WindowManager::GetInstance().Close();
    }

    ImGui::EndMenu();
  }
}
void MenuBar::__RenderViewMenu()
{
  if (ImGui::BeginMenu("View"))
  {
    auto& gui = ImGuiLayer::GetInstance();
    ImGui::MenuItem("Dear Imgui Demo", nullptr, &gui.renderImGuiDemo);
    ImGui::Separator();
    ImGui::MenuItem("Gizmo Toolbar", nullptr, &gui.toolbar.isOpen);
    ImGui::MenuItem("Time Info", nullptr, &gui.renderTimeInfo);
    ImGui::MenuItem("Graphics Info", nullptr, &gui.renderGraphicsInfo);
    ImGui::Separator();
    ImGui::MenuItem("Hierarchy", nullptr, &gui.hierarchy.isOpen);
    ImGui::MenuItem("Inspector", nullptr, &gui.inspector.isOpen);
    ImGui::EndMenu();
  }
}

