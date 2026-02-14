#include "menu_bar.hpp"
#include "imgui_layer.hpp"

#include "utils/file_dialog.hpp"
#include "engine/paths.hpp"
#include "engine/ecs/scene.hpp"
#include "engine/scene_serializer.hpp"
#include "engine/managers/window_manager.hpp"

#include <imgui.h>

void MenuBar::render()
{
  if (!scene_) 
    return;

  if (ImGui::BeginMainMenuBar())
  {
    render_file_menu();
    render_view_menu();
    ImGui::EndMainMenuBar();
  }
}

void MenuBar::render_file_menu()
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("Open"))
    {
      const char* filter_patterns[] = { "*.yaml" };
      constexpr auto nr_filter_patterns = sizeof(filter_patterns) / sizeof(filter_patterns[0]);
      auto filePath = open_file_dialog("Open new scene",
                                       GetRootPath(),
                                       nr_filter_patterns,
                                       filter_patterns,
                                       "Scene file (*yaml)",
                                       false);
      if (!filePath.empty())
      {
        scene_->Clear();
        auto serializer = SceneSerializer{ *scene_ };
        serializer.Load(filePath);
      }
    }

    if (ImGui::MenuItem("Save as..."))
    {
      const char* filter_patterns[] = { "*.yaml" };
      constexpr auto nr_filter_patterns = sizeof(filter_patterns) / sizeof(filter_patterns[0]);
      auto path = save_file_dialog("Save scene",
                                   GetRootPath(),
                                   nr_filter_patterns,
                                   filter_patterns,
                                   "Scene file (*yaml)");
      if (!path.empty())
      {
        auto serializer = SceneSerializer{ *scene_ };
        serializer.Save(path);
      }
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
    {
      WindowManager::instance().Close();
    }

    ImGui::EndMenu();
  }
}
void MenuBar::render_view_menu()
{
  if (ImGui::BeginMenu("View"))
  {
    auto& gui = ImGuiLayer::instance();
    ImGui::MenuItem("Dear Imgui Demo", nullptr, &gui.renderImGuiDemo);
    ImGui::Separator();
    ImGui::MenuItem("Gizmo Toolbar", nullptr, &gui.toolbar.is_open);
    ImGui::MenuItem("Time Info", nullptr, &gui.renderTimeInfo);
    ImGui::MenuItem("Graphics Info", nullptr, &gui.renderGraphicsInfo);
    ImGui::Separator();
    ImGui::MenuItem("Hierarchy", nullptr, &gui.hierarchy.is_open);
    ImGui::MenuItem("Inspector", nullptr, &gui.inspector.is_open);
    ImGui::EndMenu();
  }
}

