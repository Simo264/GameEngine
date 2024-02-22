#pragma once

#include "Core.hpp"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel
{
public:
	ViewportPanel(const char* panelName);

	bool isOpen;
	bool isFocused;
	String panelName;
	Vec2i viewportSize;

	void RenderPanel(class FrameBuffer* framebuffer);
};
