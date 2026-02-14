#pragma once

#include "core/basic_types.hpp"
#include "engine/ecs/scene.hpp"
#include "engine/graphics/framebuffer.hpp"

#include <chrono>

class Engine
{
public:
	Engine();
	~Engine();
	
	void run();

private:
	void setup_opengl_debug() const;
	void setup_opengl_state() const;
	void init_time();
	void calculate_per_frame_time();
	void create_camera_ubo(i32 binding_point);
	void create_light_ubo(i32 binding_point);
	Components::StaticMesh create_mesh_quad();
	void create_msaa_framebuffer(i32 width, i32 height, i32 samples);
	void resolve_msaa_framebuffer(i32 width, i32 height);

	Framebuffer msaa_framebuffer_;
	Framebuffer msaa_framebuffer_resolver_;
	i32 msaa_framebuffer_samples_;
	Components::StaticMesh screen_quad_;
	Buffer ubo_camera_block_;	// UBO "CameraBlock"	-> bindpoint 0
	Buffer ubo_light_block_;	// UBO "LightBlock"		-> bindpoint 1
	
	Scene scene_;
	
	std::chrono::steady_clock::time_point now_;
	std::chrono::steady_clock::time_point last_frame_time_;
	std::chrono::steady_clock::time_point timer_T0_;
	std::chrono::steady_clock::time_point timer_T1_;
	u32 frames_;
	u32 frame_rate_; // How many frames generated per seconds
	f64 total_deltas_per_second_;
	f64 avg_time_; // The average rendering time per seconds
	f64 delta_; // Time elapsed between two frames
};
