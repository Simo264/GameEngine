#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Core/Singleton.hpp"

#include "MenuBar.hpp"
#include "ViewportPanel.hpp"
#include "ToolbarPanel.hpp"
#include "HierarchyPanel.hpp"
#include "InspectorPanel.hpp"

struct Texture2D;
class Scene;
class Entity;

/**
 * @brief Manages rendering and integration of ImGui into the application
 */
class ImGuiLayer : public Singleton<ImGuiLayer>
{
public:
	/** @brief Initializes the ImGui context, styling, and font settings. */
	void InitializeImGui();
	/** @brief Cleans up and shuts down the ImGui context and related resources. */
	void CleanupImGui();
	/** @brief Prepares a new ImGui frame and configures the dockspace. */
	void PrepareImGuiFrame();
	/** @brief Finalizes the current ImGui frame and handles rendering. */
	void CompleteFrameRender();
	/** @brief Displays the ImGui demo window. */
	void ImguiDemo();
	
	void DebugInfo(f64 delta, f64 avg, i32 frameRate);
	
	void GraphicsInfo();

	MenuBar menubar;
	ViewportPanel viewport;
	ToolbarPanel toolbar;
	HierarchyPanel hierarchy;
	InspectorPanel inspector;

	bool renderImGuiDemo;
	bool renderTimeInfo;
	bool renderGraphicsInfo;

private:
	void __SetupImGuiContext();
	void __CustomizeStyle();
	void __ConfigureDockspace();
};