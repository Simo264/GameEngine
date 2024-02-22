#include "InspectorPanel.hpp"

#include "Imgui/imgui.h"

InspectorPanel::InspectorPanel(const char* panelName)
{
	this->panelName = panelName;
}

void InspectorPanel::RenderPanel()
{
	ImGui::Begin(panelName.c_str(), &isOpen);

	ImGui::End();
}
