#include "InspectorPanel.hpp"

#include <imgui/imgui.h>

InspectorPanel::InspectorPanel(const char* panelName)
{
	this->panelName = panelName;
}

void InspectorPanel::RenderPanel()
{
	ImGui::Begin(panelName.c_str(), &isOpen);

	ImGui::End();
}
