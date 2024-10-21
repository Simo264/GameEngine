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
	 * @return
	 * The instance of this ImGuiLayer singleton class
	 */
	static ImGuiLayer& Get()
	{
		static ImGuiLayer gui;
		return gui;
	}

	/**
	 * @brief
	 * Initialize ImGui
	 */
	void Initialize();

	/**
	 * @brief
	 * Destroy ImGui context
	 */
	void CleanUp();

	/**
	 * @brief
	 * Start the ImGui frame. To call at the start of render loop
	 */
	void BeginFrame();

	/**
	 * @brief
	 * GUI rendering. To call at the end of render loop
	 */
	void EndFrame();

	/**
	 * @brief
	 * Set ImGui font
	 */
	void SetFont(const fs::path& ttfFilePath) const;

	/**
	 * @brief 
	 * Render the menu bar
	 */
	void MenuBar(Scene& scene) const;

	/**
	 * @brief
	 * Render the demo
	 */
	void Demo();

	/**
	 * @brief
	 * Render the viewport panel
	 */
	void Viewport(u32 tetxureID, GameObject& objSelected, const mat4f& view, const mat4f& proj) const;

	/**
	 * @brief
	 * Render the outliner panel
	 */
	GameObject& Hierarchy(Scene& scene);

	/**
	 * @brief
	 * Render the inspector panel
	 */
	void Inspector(GameObject& object);

	/**
	 * @brief
	 * Render the content browser panel
	 */
	void ContentBrowser();

	/**
	 * @brief
	 * Render the gizmo tool bar
	 */
	void GizmoToolBar(GameObject& objSelected);

	/**
	 * @brief
	 * Render the camera props panel
	 */
	void CameraProps(Camera& camera);

	/**
	 * @brief
	 * Render the depth map image into a window
	 */
	void DebugDepthMap(u32 tetxureID);

	/**
	 * @brief
	 * Render the graphics informations window
	 */
	void GraphicsInfo();

	/**
	 * @brief
	 * Render the time info window
	 */
	void TimeInfo(f64 delta, f64 avg, i32 frameRate);

	/**
	 * @brief
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