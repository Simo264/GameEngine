#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Managers/ShadersManager.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <imgui.h>

void GUI_RenderMenuBar(Scene &scene)
{
  // Render menu bar
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open"))
      {
        const char *filterPatterns[] = { "*.yaml" };
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

      ImGui::Separator();

      if (ImGui::MenuItem("Exit"))
        WindowManager::GetInstance().Close();

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Build"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Select"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Actor"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help"))
    {
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}
