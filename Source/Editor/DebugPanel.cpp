#include "DebugPanel.hpp"

#include "Core/Math/Math.hpp"

#include "Engine/Graphics/Renderer.hpp"

#include <imgui/imgui.h>

extern Vec3f lightPosition;

DebugPanel::DebugPanel(const char* panelName, bool visible)
	: Panel(panelName, visible)
{
	ImGuiIO& io = ImGui::GetIO();
	_t1 = SystemClock::now();
	_t2 = SystemClock::now();
	_framerate = io.Framerate;
}

void DebugPanel::RenderFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking;
	ImGui::SetNextWindowBgAlpha(0.15f); /* Transparent background */

	//ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;

	ImGui::Begin(panelName.c_str(), &visible, flags);

	_t2 = SystemClock::now();
	const std::chrono::duration<double> diff = _t2 - _t1;
	if (diff.count() >= 0.125f) /* display every 0.125s */
	{
		_t1 = _t2;
		_framerate = io.Framerate;
	}

	ImGui::Text("%.3f ms/frame (%d FPS)", 1000.0f / _framerate, (int)_framerate);
	ImGui::Text("Draw calls: %d", Renderer::drawCalls);

	ImGui::DragFloat3("Light position", (float*)&lightPosition, 0.01f,-100.0f, 100.0);

	ImGui::End();
}
