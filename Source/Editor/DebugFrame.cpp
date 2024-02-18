#include "DebugFrame.hh"

#include "../Renderer.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"

DebugFrame::DebugFrame(const char* frameName)
{
	this->frameName = frameName;
	isOpen = false;

	ImGuiIO& io = ImGui::GetIO();
	_timeOld = 0.0f;
	_timeNew = 0.0f;
	_framerate = io.Framerate;
}

void DebugFrame::RenderFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowBgAlpha(0.15f); /* Transparent background */
	ImGui::Begin(frameName.c_str(), &isOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking);

	_timeNew = glfwGetTime();
	if (_timeNew - _timeOld >= (1.0f / 8.0f)) /* update every 1/4 seconds */
	{
		_timeOld = _timeNew;
		_framerate = io.Framerate;
	}

	ImGui::Text("%.3f ms/frame (%d FPS)", 1000.0f / _framerate, (int)_framerate);
	ImGui::Text("Draw calls: %d", Renderer::drawCalls);
	ImGui::End();
}
