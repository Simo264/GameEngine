#pragma once

#include "panel_base.hpp"

class MenuBar : public PanelBase
{
public:
	MenuBar() = default;

	void render();
private:
	void render_file_menu();
	void render_view_menu();
};

