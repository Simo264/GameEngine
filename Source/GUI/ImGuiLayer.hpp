#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Scene;
class GameObject;
class Camera;

namespace ImGuiLayer
{
	/**
	 * Set up ImGui context
	 */
	void SetupContext();

	/**
	 * Destroy ImGui context
	 */
	void CleanUp();

	void BeginFrame();
	void EndFrame();
	
	void SetFont(const fs::path& fontpath, i32 fontsize);
	
	/**
	 * Set the dockspace (call every frame!)
	 */
	void Docking();
	
	/**
	 * Render the menu bar
	 */
	void MenuBar(Scene& scene);

	/**
	 * Render the demo frame
	 */
	void Demo();
	
	/**
	 * Render the viewport with gizmo frame
	 */
	vec2i32 ViewportGizmo(u32 tetxureID, GameObject& object, const mat4f& view, const mat4f& proj);
	
	/**
	 * Render the outliner frame
	 */
	GameObject OutlinerPanel(Scene& scene);
	
	/**
	 * Render the game object details frame
	 */
	void GameObjectDetails(GameObject& object);
	
	/**
	 * Render the world props frame
	 */
	void WorldProps();
	
	/**
	 * Render the camera props frame
	 */
	void CameraProps(const char* label, Camera& camera);
	
	/**
	 * Render the depth map image into a frame
	 */
	void DebugDepthMap(u32 tetxureID);
	
	/**
	 * Render the application info frame
	 */
	void ApplicationInfo(f64 delta, f64 avg, i32 frameRate);

	/**
	 * The test frame
	 */
	void Test();
}
