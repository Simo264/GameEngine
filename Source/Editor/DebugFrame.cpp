#include "DebugFrame.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include <imgui/imgui.h>

DebugFrame::DebugFrame(const char* frameName)
{
	this->frameName = frameName;
	isOpen = true;

	ImGuiIO& io = ImGui::GetIO();
	_t1 = SystemClock::now();
	_t2 = SystemClock::now();
	_framerate = io.Framerate;
}

void DebugFrame::RenderFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowBgAlpha(0.15f); /* Transparent background */
	ImGui::Begin(frameName.c_str(), &isOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking);

	_t2 = SystemClock::now();
	const std::chrono::duration<double> diff = _t2 - _t1;
	if (diff.count() >= 0.125f) /* display every 0.125s */
	{
		_t1 = _t2;
		_framerate = io.Framerate;
	}

	ImGui::Text("%.3f ms/frame (%d FPS)", 1000.0f / _framerate, (int)_framerate);
	ImGui::Text("Draw calls: %d", Renderer::drawCalls);
	ImGui::End();
}
