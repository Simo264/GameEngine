#include "Core/Core.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Core/Paths/Paths.hpp"

#include "ImGuiLayer.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <imgui.h>

struct GoochParams
{
  f32 warmFactor;     // alpha: warm contribution
  f32 coolFactor;     // beta: cool contribution
  f32 baseCoolColor;  // b: cool color base
  f32 baseWarmColor;  // y: warm color base
};

static void MenuBar_FileItemOpen(Scene& scene)
{
  if (ImGui::MenuItem("Open"))
  {
    const char* filterPatterns[] = { "*.yaml" };
    constexpr i32 nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
    fs::path filePath = FileDialog::OpenFileDialog("Open new scene",
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
    constexpr i32 nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
    fs::path path = FileDialog::SaveFileDialog("Save scene",
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

static void MenuBar_Shading()
{
  if (ImGui::BeginMenu("Shading"))
  {
    if (ImGui::MenuItem("Blinn-Phong", nullptr, g_activeShadingModel == 0))
      g_activeShadingModel = 0;

    if (ImGui::MenuItem("Gooch", nullptr, g_activeShadingModel == 1))
      g_activeShadingModel = 1;

    ImGui::EndMenu();
  }
}

static void MenuBar_Edit(GoochParams& goochParams)
{
  if (ImGui::BeginMenu("Edit"))
  {
    if (ImGui::BeginMenu("Gooch Parameters", g_activeShadingModel == 1))
    {
      ImGui::Text("Gooch Params:");
      ImGui::SliderFloat("Warm Factor", &goochParams.warmFactor, 0.0f, 1.0f, "Alpha: %.2f");
      ImGui::SliderFloat("Cool Factor", &goochParams.coolFactor, 0.0f, 1.0f, "Beta: %.2f");
      ImGui::SliderFloat("Base Cool Color", &goochParams.baseCoolColor, 0.0f, 1.0f, "B: %.2f");
      ImGui::SliderFloat("Base Warm Color", &goochParams.baseWarmColor, 0.0f, 1.0f, "Y: %.2f");
      ImGui::EndMenu();
    }

    ImGui::EndMenu();
  }
}

static void MenuBar_Debug()
{
  if (ImGui::BeginMenu("Debug"))
  {
    ImGui::MenuItem("Normal map", nullptr, &g_renderWithNormalMapping);
    ImGui::EndMenu();
  }
}


// --------------------------
//          PUBLIC
// --------------------------

void GUI_MenuBar(Scene &scene, GoochParams& goochParams)
{
  if (ImGui::BeginMainMenuBar())
  {
    MenuBar_File(scene);
    
    MenuBar_View();

    MenuBar_Shading();

    MenuBar_Edit(goochParams);

    MenuBar_Debug();

    ImGui::EndMainMenuBar();
  }
}
