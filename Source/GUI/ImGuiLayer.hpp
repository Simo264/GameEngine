#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class Camera;
class Scene;
class GameObject;
class Animator;

/**
 * 
 * @class ImGuiLayer
 * 
 * @brief Manages rendering and integration of ImGui into the application
 * 
 * This class implements the singleton pattern to ensure only one instance of ImGuiLayer
 * exists throughout the application.
 */
class ImGuiLayer
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ImGuiLayer(const ImGuiLayer&) = delete;
	/** @brief Deleted asignment constructor to enforce singleton pattern. */
	void operator=(const ImGuiLayer&) = delete;

	/** @brief Retrieves the singleton instance of ImGuiLayer. */
	static ImGuiLayer& Get()
	{
		static ImGuiLayer gui;
		return gui;
	}

	/**
	 * @brief Initializes the ImGui context and loads the default font.
	 * 
	 * This method sets up the ImGui context, applies custom styling,
	 * and loads the font specified in the configuration file. It is executed
	 * at the start of the GUI system.
	 */
	void Initialize();

	/**
	 * @brief Performs cleanup of the ImGui context.
	 * 
	 * This method handles the removal and deallocation of all resources
	 * allocated by ImGui during execution, ensuring a clean shutdown
	 * of the GUI.
	 */
	void CleanUp();

	/**
	 * @brief Starts a new ImGui rendering frame.
	 * 
	 * Prepares the ImGui context for the new drawing phase by initializing
	 * frame buffers and starting the necessary components for GUI element handling
	 */
	void BeginFrame();

	/**
	 * @brief Ends and presents the current ImGui frame.
	 * 
	 * Completes the rendering of ImGui elements on the screen and manages
	 * the display of platform-native windows if multi-viewport mode is enabled.
	 */
	void EndFrame();

	/**
	 * @brief Sets a new font for the user interface.
	 * 
	 * @param ttfFilePath Path to the TTF file of the font to load.
	 * 
	 * This method loads and applies a new font to the ImGui interface
	 * using the specified file. The existing font is removed, and the new one
	 * is built and bound to the ImGui context.
	 * 
	 * Call before BeginFrame()!!
	 */
	void SetFont(const fs::path& ttfFilePath) const;
	
	
	void RenderMenuBar(Scene& scene) const;
	void RenderDemo();
	void RenderViewport(u32 texture, GameObject& objSelected, const mat4f& view, const mat4f& proj) const;
	GameObject& RenderHierarchy(Scene& scene);
	void RenderInspector(GameObject& object);
	void RenderContentBrowser();
	void RenderGizmoToolBar(GameObject& objSelected);
	void RenderCameraProps(Camera& camera);
	void RenderDebugDepthMap(u32 texture);
	void RenderGraphicsInfo();
	void RenderTimeInfo(f64 delta, f64 avg, i32 frameRate);
	void RenderDebug(bool shadowMode, bool normalMode, bool wireframeMode);

	vec2i viewportSize;
	vec2i viewportPos;
	bool viewportFocused;

	i32 fontSize;
	i32 gizmode;
	
	bool changeFontFamilyFlag;
	std::pair<const char*, const fs::path*> currentFont;
	
private:
	ImGuiLayer() : 
		viewportFocused{ false },
		viewportSize{},
		viewportPos{},
		currentFont{},
		changeFontFamilyFlag{ false },
		gizmode{ -1 },
		fontSize{ 0 }
	{}
	~ImGuiLayer() = default;

	void SetupContext();
	void Styling();
	void Docking();
};