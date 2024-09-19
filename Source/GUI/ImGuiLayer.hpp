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
	void SetFont(StringView fontPath);

	/**
	 * Render the menu bar
	 */
	void MenuBar(Scene& scene);

	/**
	 * Render the demo
	 */
	void Demo();

	/**
	 * Render the viewport panel
	 */
	void Viewport(u32 tetxureID, GameObject& objSelected, const mat4f& view, const mat4f& proj);

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
	 * Render the camera props panel
	 */
	void CameraProps(Camera& camera);

	/**
	 * Render the depth map image into a window
	 */
	void DebugDepthMap(u32 tetxureID);

	/**
	 * Render the debug window
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