#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Core/DesignPatterns/Singleton.hpp"

class Camera;
class Scene;
class GameObject;
class Animator;

/**
 * @brief Manages rendering and integration of ImGui into the application
 *
 * This class implements the singleton pattern to ensure only one instance of ImGuiLayer
 * exists throughout the application.
 */
class ImGuiLayer : public Singleton<ImGuiLayer>
{
public:
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
	void SetFont(const fs::path &ttfFilePath, u32 fontSize) const;

	void RenderMenuBar(Scene &scene) const;
	void RenderDemo();
	void RenderViewport(u32 texture, GameObject &objSelected, const mat4f &view, const mat4f &proj) const;
	GameObject &RenderHierarchy(Scene &scene);
	void RenderInspector(GameObject &object);
	void RenderGizmoToolBar();
	void RenderCameraSettings(Camera &camera);
	void RenderGraphicsInfo();
	void RenderTimeInfo(f64 delta, f64 avg, i32 frameRate);
	void RenderDebug(bool& wireframe,
									 bool& normalMapping,
									 u32& shadingModel,
									 f32& b,
									 f32& y,
									 f32& alpha,
									 f32& beta);

	vec2i viewportSize;
	vec2i viewportPos;
	bool viewportFocused;

	i32 gizmode;

private:
	void SetupContext();
	void Styling();
	void Docking();
};