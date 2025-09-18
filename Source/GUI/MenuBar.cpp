#include "Core/Core.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Core/Paths/Paths.hpp"

#include "ImGuiLayer.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <imgui.h>

static void MenuBar_FileItemOpen(Scene& scene)
{
  if (ImGui::MenuItem("Open"))
  {
    const char* filterPatterns[] = { "*.yaml" };
    constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
    auto filePath = FileDialog::OpenFileDialog("Open new scene",
                                               Paths::GetRootPath(),
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
}
static void MenuBar_FileItemSaveAs(Scene& scene)
{
  if (ImGui::MenuItem("Save as..."))
  {
    const char* filterPatterns[] = { "*.yaml" };
    constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
    auto path = FileDialog::SaveFileDialog("Save scene",
                                           Paths::GetRootPath(),
                                           nrFilterPatterns,
                                           filterPatterns,
                                           "Scene file (*yaml)");
    if (!path.empty())
      scene.SaveToFile(path);
  }
}
static void MenuBar_FileItemExit()
{
  if (ImGui::MenuItem("Exit"))
    WindowManager::GetInstance().Close();
}
static void MenuBar_File(Scene& scene)
{
  if (ImGui::BeginMenu("File"))
  {
    MenuBar_FileItemOpen(scene);
    
    MenuBar_FileItemSaveAs(scene);

    ImGui::Separator();

    MenuBar_FileItemExit();

    ImGui::EndMenu();
  }
}

static void MenuBar_View()
{
  if (ImGui::BeginMenu("View"))
  { 
    auto& gui = ImGuiLayer::GetInstance();
    ImGui::MenuItem("Dear Imgui Demo", nullptr, &gui.renderImGuiDemo);
    ImGui::Separator();
    ImGui::MenuItem("Gizmo Toolbar", nullptr, &gui.renderToolbar);
    ImGui::MenuItem("Time Info", nullptr, &gui.renderTimeInfo);
    ImGui::MenuItem("Graphics Info", nullptr, &gui.renderGraphicsInfo);
    ImGui::Separator();
    ImGui::MenuItem("Hierarchy", nullptr, &gui.renderHierarchy);
    ImGui::MenuItem("Inspector", nullptr, &gui.renderInspector);
    ImGui::MenuItem("Camera Properties", nullptr, &gui.renderCameraProperties);

    ImGui::EndMenu();
  }
}

// --------------------------
//          PUBLIC
// --------------------------

void GUI_MenuBar(Scene &scene)
{
  if (ImGui::BeginMainMenuBar())
  {
    MenuBar_File(scene);
    
    MenuBar_View();

    ImGui::EndMainMenuBar();
  }
}
