#include "MenuBar.hpp"
#include "ImGuiLayer.hpp"

#include "Utils/FileDialog.hpp"
#include "Engine/Paths.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/SceneSerializer.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <imgui.h>

void MenuBar::Render()
{
  if (!_scene) 
    return;

  if (ImGui::BeginMainMenuBar())
  {
    __RenderFileMenu();
    __RenderViewMenu();
    ImGui::EndMainMenuBar();
  }
}

void MenuBar::__RenderFileMenu()
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Open"))
    {
      const char* filterPatterns[] = { "*.yaml" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto filePath = FileDialog::OpenFileDialog("Open new scene",
                                                 GetRootPath(),
                                                 nrFilterPatterns,
                                                 filterPatterns,
                                                 "Scene file (*yaml)",
                                                 false);
      if (!filePath.empty())
      {
        _scene->Clear();
        auto serializer = SceneSerializer{ *_scene };
        serializer.Load(filePath);
      }
    }

    if (ImGui::MenuItem("Save as..."))
    {
      const char* filterPatterns[] = { "*.yaml" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto path = FileDialog::SaveFileDialog("Save scene",
                                             GetRootPath(),
                                             nrFilterPatterns,
                                             filterPatterns,
                                             "Scene file (*yaml)");
      if (!path.empty())
      {
        auto serializer = SceneSerializer{ *_scene };
        serializer.Save(path);
      }
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

