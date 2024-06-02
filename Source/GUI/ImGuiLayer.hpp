#pragma once

#include "Core/Core.hpp"

class Scene;
class GameObject;

namespace ImGuiLayer
{
	void SetupContext();
	void CleanUp();
	
	void SetFont(const fspath& fontpath, int fontsize);
	
	void NewFrame();
	void DrawData();
	void Docking();
	
	void RenderMenuBar();
	GameObject RenderOutlinerPanel(Scene& scene);
	void RenderDetails(GameObject object);
	void RenderDemo();
}
