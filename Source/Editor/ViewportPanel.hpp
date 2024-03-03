#pragma once

#include "Editor/Panel.hpp"
#include "Core/Math/Math.hpp"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel : public Panel
{
public:
	ViewportPanel(const char* panelName, bool visible)
		: Panel(panelName, visible),
		isFocused{ false },
		viewportSize{ 0, 0 } /* Set size on Render() function */
	{}
	
	bool isFocused;
	Vec2i viewportSize;

	void RenderPanel(class FrameBuffer* framebuffer);
};
