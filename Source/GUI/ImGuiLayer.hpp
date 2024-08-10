#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Scene;
class GameObject;
class TransformComponent;

class Camera;

namespace ImGuiLayer
{
	void SetupContext();
	void CleanUp();
	
	void SetFont(const fs::path& fontpath, int fontsize);
	
	void BeginFrame();
	void EndFrame();
	void Docking();
	
	void RenderDemo();
	void RenderMenuBar(Scene& scene);
	vec2i32 RenderViewportAndGuizmo(uint32_t tetxureID, GameObject& object, const mat4f& view, const mat4f& proj);
	GameObject RenderOutlinerPanel(Scene& scene);
	void RenderDetails(GameObject object);
	void RenderGlobals();
	void RenderCameraProps(const char* label, Camera& camera);
	void RenderDepthMap(uint32_t tetxureID);

	void RenderDebug(Camera& camera);
}
