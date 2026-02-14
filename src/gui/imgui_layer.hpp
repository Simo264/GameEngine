#pragma once

#include "core/basic_types.hpp"
#include "core/singleton.hpp"

#include "menu_bar.hpp"
#include "viewport_panel.hpp"
#include "toolbar_panel.hpp"
#include "hierarchy_panel.hpp"
#include "inspector_panel.hpp"

class Scene;

/**
 * @brief Manages rendering and integration of ImGui into the application
 */
class ImGuiLayer : public Singleton<ImGuiLayer>
{
public:
	/** @brief Initializes the ImGui context, styling, and font settings. */
	void initialize_imgui(Scene& scene);
	/** @brief Cleans up and shuts down the ImGui context and related resources. */
	void cleanup_imgui();
	/** @brief Prepares a new ImGui frame and configures the dockspace. */
	void prepare_imgui_frame();
	/** @brief Finalizes the current ImGui frame and handles rendering. */
	void complete_frame_render();
	/** @brief Displays the ImGui demo window. */
	void imgui_demo();
	
	void debug_info(f64 delta, f64 avg, i32 frameRate);
	
	void graphics_info();

	MenuBar menubar;
	ViewportPanel viewport;
	ToolbarPanel toolbar;
	HierarchyPanel hierarchy;
	InspectorPanel inspector;

	bool render_imgui_demo;
	bool render_time_info;
	bool render_graphics_info;

private:
	void setup_imgui_context();
	void customize_style();
	void configure_dockspace();
};