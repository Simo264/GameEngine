#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

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
	
	void RenderMenuBar(Scene& scene);
	GameObject RenderOutlinerPanel(Scene& scene);
	void RenderDetails(GameObject object);
	void RenderDemo();

}
