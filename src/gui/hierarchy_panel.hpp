#pragma once

#include "core/basic_types.hpp"
#include "engine/ecs/entity.hpp"
#include "panelbase.hpp"

#include <string_view>

class HierarchyPanel : public PanelBase
{
public:
  HierarchyPanel() : is_open{ true }, target{}{}

	void render();

	bool is_open;
  Entity target;

private:
  bool button_centered(std::string_view label, f32 width, f32 height);
  void render_new_entity_modal();
  void render_entity_list();
};
