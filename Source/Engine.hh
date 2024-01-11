#pragma once

#include "Core.hh"
#include "Editor/Editor.hh"

#define INITIAL_WINDOW_SIZE_X 1366 // 16:9 aspect ratio resolution
#define INITIAL_WINDOW_SIZE_Y 768

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
	void LoadShaders();
};
