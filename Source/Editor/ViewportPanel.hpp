#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

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
