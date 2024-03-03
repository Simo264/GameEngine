#include "SettingsPanel.hpp"
#include "Core/Math/Math.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

struct WindowArgs 
{
  String title;
  Vec2i position;
  Vec2i resolution;
  Vec2i aspectratio;
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
  _windowArgs->position = INIFileParser::StringToVec2<Vec2i>(_conf->GetValue("window", "position"), ',');
  _windowArgs->resolution = INIFileParser::StringToVec2<Vec2i>(_conf->GetValue("window", "resolution"), 'x');
  _windowArgs->aspectratio = INIFileParser::StringToVec2<Vec2i>(_conf->GetValue("window", "aspectratio"), ':');
  _windowArgs->vsync = INIFileParser::StringToBool(_conf->GetValue("window", "vsync"));

  String resolutionStr = _conf->GetValue("window", "resolution");
  String aspectRatioStr = _conf->GetValue("window", "aspectratio");
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
  static bool buttonDisabled = true;
  ImGui::Begin(panelName.c_str(), &visible, ImGuiWindowFlags_NoDocking);
  ImGui::SeparatorText("Window properties");

  /* Window title */
  if (ImGui::InputText("Title", &_windowArgs->title))
    buttonDisabled = false;

  /* Window aspect ratio */
  if (ImGui::Combo("Aspect ratio", &_aspectIndex, &_aspectRatioValues[0], IM_ARRAYSIZE(_aspectRatioValues)))
  {
    _windowArgs->aspectratio = INIFileParser::StringToVec2<Vec2i>(_aspectRatioValues[_aspectIndex], ':');
    buttonDisabled = false;
  }

  /* Window resolution */
  if (ImGui::Combo("Resolution", &_resolutionIndex, _resolutionValues, IM_ARRAYSIZE(_resolutionValues)))
  {
    _windowArgs->resolution = INIFileParser::StringToVec2<Vec2i>(_resolutionValues[_resolutionIndex], 'x');
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
  _conf->Update("window", "title", _windowArgs->title.c_str());
  _conf->Update("window", "resolution", INIFileParser::Vec2ToString<Vec2i>(_windowArgs->resolution, 'x').c_str());
  _conf->Update("window", "position", INIFileParser::Vec2ToString<Vec2i>(_windowArgs->position, ',').c_str());
  _conf->Update("window", "aspectratio", INIFileParser::Vec2ToString<Vec2i>(_windowArgs->aspectratio, ':').c_str());
  _conf->Update("window", "vsync", INIFileParser::BoolToString(_windowArgs->vsync).c_str());
  _conf->Write();
}