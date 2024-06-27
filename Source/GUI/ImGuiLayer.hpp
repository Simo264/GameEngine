#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Scene;
class GameObject;
class TransformComponent;

namespace ImGuiLayer
{
	void SetupContext();
	void CleanUp();
	
	void SetFont(const fspath& fontpath, int fontsize);
	
	void BeginFrame();
	void EndFrame();
	void Docking();
	
	void RenderMenuBar(Scene& scene);
	GameObject RenderOutlinerPanel(Scene& scene);
	void RenderDetails(GameObject object);
	void RenderDemo();
}
