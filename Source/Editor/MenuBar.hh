#pragma once

#include "../Core.hh"

enum MENU_ITEMS : int {
	PANEL_OUTLINER = 0,
	PANEL_BROWSER,
	PANEL_INSPECTOR,
	PANEL_VIEWPORT,
	PANEL_DETAILS,
	FRAME_DEBUG,
	FRAME_DEMO,
	FRAME_SETTINGS
};

/* ----------------------------------------
			Menu Bar class
	---------------------------------------- */
class MenuBar
{
public:
	MenuBar(const Array<bool*, 8>& menuItems);
	~MenuBar() = default;

	void RenderMenuBar();

	Array<bool*, 8> items;
};
