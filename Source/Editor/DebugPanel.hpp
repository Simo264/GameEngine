#pragma once

#include "Editor/Panel.hpp"

/* ----------------------------------------
			DebugFrame class
	---------------------------------------- */
class DebugPanel : public Panel
{
public:
	DebugPanel(const char* panelName, bool visible);

	void RenderFrame();

private:
	TimePoint _t1;
	TimePoint _t2;
	double _framerate;
};
