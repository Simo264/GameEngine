#pragma once

#include "Core.hh"
#include "Editor/Editor.hh"

#define INITIAL_WINDOW_SIZE_X 1280
#define INITIAL_WINDOW_SIZE_Y 1080

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
