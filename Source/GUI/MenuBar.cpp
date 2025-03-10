#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Subsystems/ShadersManager.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui/imgui.h>

void GUI_RenderMenuBar(Scene& scene, bool& openPreferences)
{
  // Render menu bar
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open"))
      {
        static const char* filter[] = { "*.ini" };

        fs::path filePath = Utils::OpenFileDialog(1, filter, "Open scene", false);
        if (!filePath.empty())
        {
          // Unset all lights
          ShadersManager& shadersManager = ShadersManager::Get();
          Program shaderScene = shadersManager.GetProgram("Scene");
          Program shaderSceneShadows = shadersManager.GetProgram("SceneShadows");
          shaderScene.SetUniform1f("u_directionalLight.intensity", 0.f);
          shaderScene.SetUniform1f("u_pointLight.intensity", 0.f);
          shaderScene.SetUniform1f("u_spotLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_directionalLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_pointLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_spotLight.intensity", 0.f);

          scene.Clear();
          scene.LoadFromFile(filePath);
        }
      }
      if (ImGui::MenuItem("Save as..."))
      {
        const char* filters[] = { "*.ini" };
        fs::path filepath = Utils::SaveFileDialog(1, filters, "Save as .ini");
        
        scene.Save(filepath);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit"))
        WindowManager::Get().Close();

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window"))
    {
      if (ImGui::MenuItem("Preferences"))
        openPreferences = true;

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
