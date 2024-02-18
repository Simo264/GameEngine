#include "InspectorPanel.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"

InspectorPanel::InspectorPanel(const char* panelName)
{
	this->panelName = panelName;
}

void InspectorPanel::RenderPanel()
{
	ImGui::Begin(panelName.c_str(), &isOpen);

	ImGui::End();
}
