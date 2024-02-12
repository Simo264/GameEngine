#include "SettingsFrame.hh"

#include "../Subsystems/ConfigurationsManager.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

SettingsFrame::SettingsFrame(const char* frameName)
{
	this->frameName = frameName;
	isOpen = false;

  auto& instanceCM = ConfigurationsManager::Instance();
  String& resolutionStr = instanceCM.GetValue(CONF_WINDOW_RESOLUTION);
  String& aspectRatioStr = instanceCM.GetValue(CONF_ASPECT_RATIO);
  for (_aspectIndex = 0; _aspectIndex < IM_ARRAYSIZE(_aspectRatioValues); _aspectIndex++)
    if (std::strcmp(_aspectRatioValues[_aspectIndex], aspectRatioStr.c_str()) == 0)
      break;
  for (_resolutionIndex = 0; _resolutionIndex < IM_ARRAYSIZE(_resolutionValues); _resolutionIndex++)
    if (std::strcmp(_resolutionValues[_resolutionIndex], resolutionStr.c_str()) == 0)
      break;
}

void SettingsFrame::RenderFrame()
{
  auto& instanceCM = ConfigurationsManager::Instance();

  static bool buttonDisabled = true;
  ImGui::Begin(frameName.c_str(), &isOpen);
  ImGui::SeparatorText("Window properties");

  /* Window title */
  static String& title = instanceCM.GetValue(CONF_WINDOW_TITLE);
  if (ImGui::InputText("Title", &title))
    buttonDisabled = false;

  /* Window aspect ratio */
  if (ImGui::Combo("Aspect ratio", &_aspectIndex, &_aspectRatioValues[0], IM_ARRAYSIZE(_aspectRatioValues)))
  {
    instanceCM.SetValue(CONF_ASPECT_RATIO, _aspectRatioValues[_aspectIndex]);
    buttonDisabled = false;
  }

  /* Window resolution */
  if (ImGui::Combo("Resolution", &_resolutionIndex, _resolutionValues, IM_ARRAYSIZE(_resolutionValues)))
  {
    instanceCM.SetValue(CONF_WINDOW_RESOLUTION, _resolutionValues[_resolutionIndex]);
    buttonDisabled = false;
  }

  /* V-sync */
  static bool vsyncActive = (std::strcmp(instanceCM.GetValue(CONF_VSYNC).c_str(), "true") == 0);
  if (ImGui::Checkbox("V-sync", &vsyncActive))
  {
    instanceCM.SetValue(CONF_VSYNC, (vsyncActive ? "true" : "false"));
    buttonDisabled = false;
  }

  /* Disabling button */
  if (buttonDisabled)
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
  }
  /* Button event */
  bool save = ImGui::Button("Save");
  if (save)
    instanceCM.Save();
  /* Restore button */
  if (buttonDisabled)
  {
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
  }
  
  if (save)
  {
    ImGui::OpenPopup("Save?");
    buttonDisabled = true;
  }

  /* Always center this window when appearing */
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::Text("Changes will be made when the application is restarted");
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0)))
      ImGui::CloseCurrentPopup(); 

    ImGui::EndPopup();
  }


  ImGui::End();
}
