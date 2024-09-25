#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Camera.hpp"
#include "Engine/ECS/ECS.hpp"
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
	 * Initialize ImGui
	 */
	void Initialize();

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
	void SetFont(StringView fontPath) const;

	/**
	 * Render the menu bar
	 */
	void MenuBar(Scene& scene) const;

	/**
	 * Render the demo
	 */
	void Demo();

	/**
	 * Render the viewport panel
	 */
	void Viewport(u32 tetxureID, GameObject& objSelected, const mat4f& view, const mat4f& proj) const;

	/**
	 * Render the outliner panel
	 */
	GameObject& Hierarchy(Scene& scene);

	/**
	 * Render the inspector panel
	 */
	void Inspector(GameObject& object);

	/**
	 * Render the content browser panel
	 */
	void ContentBrowser();

	/**
	 * Render the gizmo tool bar
	 */
	void GizmoToolBar(GameObject& objSelected);

	/**
	 * Render the camera props panel
	 */
	void CameraProps(Camera& camera);

	/**
	 * Render the depth map image into a window
	 */
	void DebugDepthMap(u32 tetxureID);

	/**
	 * Render the graphics informations window
	 */
	void GraphicsInfo();

	/**
	 * Render the time info window
	 */
	void TimeInfo(f64 delta, f64 avg, i32 frameRate);

	/**
	 * Render the debug frame
	 */
	void Debug(bool shadowMode, bool normalMode, bool wireframeMode);

	vec2i32 viewportSize;
	vec2i32 viewportPos;
	bool viewportFocused;

	i32 fontSize;
	i32 gizmode;

	/**
	 * First: pointer to the FontManager key
	 * Second: pointer to the FontManager value 
	 */
	std::pair<String*, fs::path*> selectedFont;

	bool changeFontFamily;

private:
	ImGuiLayer();
	~ImGuiLayer() = default;

	void SetupContext();
	void Styling();
	void Docking();
};