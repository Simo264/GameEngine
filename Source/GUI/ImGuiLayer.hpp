#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Scene;
class GameObject;
class TransformComponent;
class Texture2D;

namespace ImGuiLayer
{
	void SetupContext();
	void CleanUp();
	
	void SetFont(const fspath& fontpath, int fontsize);
	
	void BeginFrame();
	void EndFrame();
	void Docking();
	
	void RenderDemo();
	void RenderMenuBar(Scene& scene);
	vec2i32 RenderViewportAndGuizmo(const Texture2D& image, GameObject& object, const mat4f& view, const mat4f& proj);
	GameObject RenderOutlinerPanel(Scene& scene);
	void RenderDetails(GameObject object);
	void RenderTesting();
}
