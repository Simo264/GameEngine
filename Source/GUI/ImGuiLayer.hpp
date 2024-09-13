#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Components.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Camera.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

class ImGuiLayer
{
public:
	ImGuiLayer(const ImGuiLayer&) = delete;
	void operator=(const ImGuiLayer&) = delete;

	/**
	 * Return the instance of this ImGuiLayer singleton class
	 */
	static ImGuiLayer& Get()
	{
		static ImGuiLayer gui;
		return gui;
	}

	/**
	 * Set up ImGui context
	 */
	void SetupContext();

	/**
	 * Destroy ImGui context
	 */
	void CleanUp();

	/**
	 * Start the ImGui frame. To call at the start of render loop
	 */
	void BeginFrame();

	/**
	 * GUI rendering. To call at the end of render loop
	 */
	void EndFrame();

	/**
	 * Set ImGui font
	 */
	void SetFont(const fs::path& fontpath, i32 fontsize);

	/**
	 * Set ImGui style
	 */
	void SetStyle();

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
	void ViewportGizmo(u32 tetxureID, GameObject& object, const mat4f& view, const mat4f& proj);

	/**
	 * Render the outliner frame
	 */
	void OutlinerPanel(Scene& scene);

	/**
	 * Render the game object details frame
	 */
	void GameObjectDetails(GameObject& object);

	/**
	 * Render the world props frame
	 */
	void WorldProps();

	/**
	 * Render the content browser frame
	 */
	void ContentBrowser();

	/**
	 * Render the camera props frame
	 */
	void CameraProps(const char* label, Camera& camera);

	/**
	 * Render the depth map image into a frame
	 */
	void DebugDepthMap(u32 tetxureID);

	/**
	 * Render the debug frame
	 */
	void DebugInfo(f64 delta, 
		f64 avg, 
		i32 frameRate, 
		bool shadowMode, 
		bool normalMode, 
		bool wireframeMode);

	/**
	 * The test frame
	 */
	void Test();

	vec2i32 viewportSize;
	vec2i32 viewportPos;
	bool viewportFocused;
	GameObject objectSelected;

private:
	ImGuiLayer();
	~ImGuiLayer() = default;

	i32 _gizmode;

	/**
	 * Set the dockspace
	 */
	void Docking();

	void GizmoWorldTranslation(Components::Transform& transform, const mat4f& view, const mat4f& proj);
	void GizmoWorldRotation(Components::Transform& transform, const mat4f& view, const mat4f& proj);
	void GizmoWorldScaling(Components::Transform& transform, const mat4f& view, const mat4f& proj);

	void ComboTextures(Texture2D*& matTexture, const char* comboLabel);
};