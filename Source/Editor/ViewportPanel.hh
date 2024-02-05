#pragma once

#include "../Core.hh"

class ViewportPanel
{
public:
	ViewportPanel(const char* name, Vec2i size);
	~ViewportPanel() = default;

	bool isOpen;
	bool isFocused;
	Vec2i viewportSize;
	Vec2i vewportPadding;
	String panelName;

	void RenderPanel(class FrameBuffer* framebuffer);

};
