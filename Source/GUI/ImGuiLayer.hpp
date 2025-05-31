#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Core/DesignPatterns/Singleton.hpp"

class Texture2D;
class Camera;
class Scene;
class GameObject;

/**
 * @brief Manages rendering and integration of ImGui into the application
 */
class ImGuiLayer : public Singleton<ImGuiLayer>
{
public:
	/** @brief Initializes the ImGui context, styling, and font settings. */
	void InitializeImGui();
	/** @brief Cleans up and shuts down the ImGui context and related resources. */
	void CleanUpImGui();
	/** @brief Prepares a new ImGui frame and configures the dockspace. */
	void PrepareImGuiFrame();
	/** @brief Finalizes the current ImGui frame and handles rendering. */
	void CompleteFrameRender();


	/** @brief Displays the main menu bar of the ImGui layer. */
	void MenuBar(Scene &scene) const;
	/** @brief Displays the ImGui demo window. */
	void ImguiDemo();
	/** @brief Renders the ImGui viewport */
	void Viewport(Texture2D textureImage,
								GameObject &objSelected, 
								const Mat4f &view, 
								const Mat4f &proj) const;
	/** @brief Displays the hierarchy view. */
	GameObject& Hierarchy(Scene& scene);
	/** @brief Displays the inspector view for examining a specific game object. */
	void Inspector(GameObject& object);
	/** @brief Renders the toolbar for user interaction */
	void ToolBar();
	/** @brief Renders timing information including delta time, average time, and frame rate. */
	void TimeInfo(f64 delta, f64 avg, i32 frameRate);
	/** @brief Displays properties and controls for a camera  */
	void CameraProperties(Camera &camera);
	/** @brief Displays detailed graphics information */
	void GraphicsInfo();

	void DebugImage(Texture2D image, i32 w, i32 h);

	Vec2I viewportSize;
	Vec2I viewportPos;
	bool viewportFocused;
	i32 gizmode;

	bool renderImGuiDemo;
	bool renderToolbar;
	bool renderTimeInfo;
	bool renderHierarchy;
	bool renderInspector;
	bool renderCameraProperties;
	bool renderGraphicsInfo;

private:
	void SetupImGuiContext();
	void CustomizeStyle();
	void ConfigureDockspace();
};