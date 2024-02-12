#pragma once

#include "../Core.hh"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel
{
public:
	ViewportPanel(const char* panelName);
	~ViewportPanel() = default;

	bool isOpen;
	bool isFocused;
	String panelName;
	Vec2i viewportSize;

	void RenderPanel(class FrameBuffer* framebuffer);
};
