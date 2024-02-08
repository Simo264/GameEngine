#pragma once

#include "../Core.hh"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel
{
public:
	ViewportPanel(const char* panelName, Vec2i panelSize);
	~ViewportPanel() = default;

	bool isOpen;
	bool isFocused;
	Vec2i viewportSize;

	void RenderPanel(class FrameBuffer* framebuffer);

private:
	String _panelName;
};
