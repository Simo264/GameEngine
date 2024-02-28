#pragma once

#include "Core/Core.hpp"

/* ----------------------------------------
			DebugFrame class
	---------------------------------------- */
class DebugFrame
{
public:
	DebugFrame(const char* frameName);

	bool isOpen;
	String frameName;

	void RenderFrame();

private:
	TimePoint _t1;
	TimePoint _t2;
	double _framerate;
};
