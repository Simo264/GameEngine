#pragma once

#include "core/basic_types.hpp"
#include "core/math/matrix.hpp"
#include "panel_base.hpp"
#include <string_view>

namespace Components
{
  struct Transform;
};
struct Texture2D;
class Entity;

class ViewportPanel : public PanelBase
{
public:
  ViewportPanel() : 
    is_open{ true },
    is_focused{ false },
    width{ 0 },
    height{ 0 },
    pos_x{ 0 },
    pos_y{ 0 },
    gizmo_op{ -1 }
  {}
  
  void render(std::string_view window_name, 
              Texture2D viewport_image,
              Entity target,
              const mat4f& view, 
              const mat4f& proj);
  
  bool is_open;
  bool is_focused;
  i32 width, height;
  i32 pos_x, pos_y;
  i32 gizmo_op;

private:
  void gizmo_world_translation(Components::Transform& transform);
  void gizmo_world_rotation(Components::Transform& transform);
  void gizmo_world_scaling(Components::Transform& transform);
};