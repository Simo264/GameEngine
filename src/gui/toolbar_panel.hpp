#pragma once

#include "core/basic_types.hpp"
#include "engine/graphics/texture2d.hpp"
#include "panel_base.hpp"
#include <string_view>

class ToolbarPanel : public PanelBase
{
public:
	ToolbarPanel();

	void render(std::string_view window_name, 
	            i32 viewport_pos_x, 
	            i32 viewport_pos_y, 
	            i32 viewport_width, 
	            i32 viewport_height, 
	            i32& viewport_gizmo_op);

  bool is_open;

private:
	Texture2D expand_icon_;
	Texture2D scale_icon_;
	Texture2D rotate_icon_;
	Texture2D wireframe_icon_;

  void gizmo_button(std::string_view label,
                    Texture2D icon,
                    f32 btn_size,
                    i32& viewport_gizmo_op,
                    i32 op,
                    std::string_view tooltip);

  void wireframe_button(std::string_view label,
                        Texture2D icon,
                        f32 btn_size,
                        bool& wireframe,
                        std::string_view tooltip);
};

