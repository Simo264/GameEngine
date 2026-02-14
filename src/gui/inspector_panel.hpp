#pragma once

#include "core/basic_types.hpp"
#include "engine/graphics/texture2d.hpp"
#include "panel_base.hpp"

#include <string_view>

namespace Components
{
	struct Tag;
	struct Transform;
	struct Camera;
	struct StaticMesh;
	struct Material;
	
	struct Light;
	struct DirectionalLight;
	struct PointLight;
	struct SpotLight;
};

class Scene;
class Entity;

class InspectorPanel : public PanelBase
{
public:
	InspectorPanel();
	void render(std::string_view window_name, Entity entity);

	bool is_open;

private:
	void list_all_components(Entity entity);
	
	void render_tag_component(Components::Tag& tag) const;
	void render_transform_component(Components::Transform& transform) const;
	void render_camera_component(Components::Camera& camera) const;
	void render_static_mesh_component(Components::StaticMesh& mesh) const;
	void render_material_component(Components::Material& material) const;
	
	void render_directional_light(Components::DirectionalLight& light) const;
	void render_point_light(Components::PointLight& light) const;
	void render_spot_light(Components::SpotLight& light) const;
	
	void attenuation_preset_combo(std::string_view id, f32& kl, f32& kq) const;

	// Helper function for a 3-float slider
	void drag_3f(std::string_view label, 
							 std::string_view id,
							 f32 values[3],
							 f32 speed,
							 f32 min_val, 
							 f32 max_val, 
							 std::array<std::string_view, 3> format) const;
	
	// Helper function for a single float slider
	void drag_1f(std::string_view label, 
							 std::string_view id,
							 f32& value, 
							 f32 speed, 
							 f32 min_val, 
							 f32 max_val,
							 std::string_view format,
							 i32 flags = 0) const;
	
	// Helper function for a single float slider
	void slider_1f(std::string_view label,
								 std::string_view id,
								 f32& value,
								 f32 min_val,
								 f32 max_val,
								 std::string_view format,
								 i32 flags = 0) const;
	
	// Helper function for a Color Edit
	void color_edit_3(std::string_view label, 
										std::string_view id, 
										f32 color[3]) const;

	Texture2D reset_icon_;
};