#pragma once

#include "../Core.hh"

/* ----------------------------------------
			DebugFrame class
	---------------------------------------- */
class DebugFrame
{
public:
	DebugFrame(const char* frameName);
	~DebugFrame() = default;

	bool isOpen;
	String frameName;

	void RenderFrame();

private:
	float _timeOld;
	float _timeNew;
	float _framerate;
};
