#pragma once

#include "Core.hpp"

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
	float _timeOld;
	float _timeNew;
	float _framerate;
};
