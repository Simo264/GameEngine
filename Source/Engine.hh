#pragma once

#include "Core.hh"
#include "Editor/Editor.hh"

#define INITIAL_WINDOW_SIZE_X 1600 /* Default screen resolution for 16:9 */
#define INITIAL_WINDOW_SIZE_Y 900
#define INITIAL_WINDOW_POS_X 50
#define INITIAL_WINDOW_POS_Y 50
#define GAMMA_CORRECTION 2.2f

class Engine
{
public:
	Engine()	= default;
	~Engine() = default;

	void Initialize();
	void Run();
	void ShutDown();

	Editor editor;

private:
	void InitOpenGL();
};
