#include "SettingsPanel.hpp"
#include "Core/Math/Math.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

struct WindowArgs 
{
  string title;
  Vec2i32 position;
  Vec2i32 resolution;
  Vec2i32 aspectratio;
  bool vsync;

  WindowArgs()
  {
    title.reserve(32);
  }
};

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

SettingsPanel::SettingsPanel(const char* panelName, bool visible)
  : Panel(panelName, visible)
{
  _conf = std::make_unique<INIFileParser>(ROOT_PATH / CONFIG_FILENAME);
  _conf->ReadData();

  _windowArgs = new WindowArgs;
  _windowArgs->title = _conf->GetValue("window", "title");
  
  string value;
  value.reserve(32);

  value = _conf->GetValue("window", "position");
  _windowArgs->position = INIFileParser::StringToVec2i(value, ",");
  
  value = _conf->GetValue("window", "resolution");
  _windowArgs->resolution = INIFileParser::StringToVec2i(value, "x");
  
  value = _conf->GetValue("window", "aspectratio");
  _windowArgs->aspectratio = INIFileParser::StringToVec2i(value, ":");
  
  _windowArgs->vsync = INIFileParser::StringToBool(_conf->GetValue("window", "vsync"));

  string resolutionStr = _conf->GetValue("window", "resolution");
  string aspectRatioStr = _conf->GetValue("window", "aspectratio");
  for (_aspectIndex = 0; _aspectIndex < IM_ARRAYSIZE(_aspectRatioValues); _aspectIndex++)
    if (std::strcmp(_aspectRatioValues[_aspectIndex], aspectRatioStr.c_str()) == 0)
      break;
  for (_resolutionIndex = 0; _resolutionIndex < IM_ARRAYSIZE(_resolutionValues); _resolutionIndex++)
    if (std::strcmp(_resolutionValues[_resolutionIndex], resolutionStr.c_str()) == 0)
      break;
}

SettingsPanel::~SettingsPanel()
{
  delete _windowArgs;
}

void SettingsPanel::RenderFrame()
{
  static string buffer(32, '\0');

  static bool buttonDisabled = true;
  ImGui::Begin(panelName.c_str(), &visible, ImGuiWindowFlags_NoDocking);
  ImGui::SeparatorText("Window properties");

  /* Window title */
  if (ImGui::InputText("Title", &_windowArgs->title))
    buttonDisabled = false;

  /* Window aspect ratio */
  if (ImGui::Combo("Aspect ratio", &_aspectIndex, &_aspectRatioValues[0], IM_ARRAYSIZE(_aspectRatioValues)))
  {
    buffer = _aspectRatioValues[_aspectIndex];
    _windowArgs->aspectratio = INIFileParser::StringToVec2i(buffer, ":");
    buttonDisabled = false;
  }

  /* Window resolution */
  if (ImGui::Combo("Resolution", &_resolutionIndex, _resolutionValues, IM_ARRAYSIZE(_resolutionValues)))
  {
    buffer = _resolutionValues[_resolutionIndex];
    _windowArgs->resolution = INIFileParser::StringToVec2i(buffer, "x");
    buttonDisabled = false;
  }

  /* V-sync */
  if (ImGui::Checkbox("V-sync", &_windowArgs->vsync))
    buttonDisabled = false;

  /* Disabling button */
  if (buttonDisabled)
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
  }
  /* Button event */
  bool save = ImGui::Button("Save");
  if (save)
    OnSaveSettings();
  
  /* Restore button */
  if (buttonDisabled)
  {
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
  }
  /* Open popup */
  if (save)
  {
    ImGui::OpenPopup("Message");
    buttonDisabled = true;
  }

  /* Always center this window when appearing */
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  if (ImGui::BeginPopupModal("Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::Text("Changes will be made when the application is restarted");
    ImGui::Separator();
    if (ImGui::Button("OK", ImVec2(120, 0)))
      ImGui::CloseCurrentPopup(); 
    ImGui::EndPopup();
  }
  
  ImGui::End();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void SettingsPanel::OnSaveSettings()
{
  char buff[32]{};

  _conf->Update("window", "title", _windowArgs->title.c_str());
  
  sprintf_s(buff, "%dx%d", _windowArgs->resolution.x, _windowArgs->resolution.y);
  _conf->Update("window", "resolution", buff);

  sprintf_s(buff, "%d,%d", _windowArgs->position.x, _windowArgs->position.y);
  _conf->Update("window", "position", buff);

  sprintf_s(buff, "%d:%d", _windowArgs->aspectratio.x, _windowArgs->aspectratio.y);
  _conf->Update("window", "aspectratio", buff);

  _conf->Update("window", "vsync", (_windowArgs->vsync ? "true" : "false"));
  _conf->Write();
}
