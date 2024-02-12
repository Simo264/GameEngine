#pragma once

#include "../Core.hh"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel
{
public:
	ViewportPanel(const char* panelName, Vec2i viewportSize);
	~ViewportPanel() = default;

	bool isOpen;
	bool isFocused;
	Vec2i viewportSize;
	String panelName;

	void RenderPanel(class FrameBuffer* framebuffer);
};
