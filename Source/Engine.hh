#pragma once

#include "Core.hh"
#include "Editor/Editor.hh"
#include "Graphics/FrameBuffer.hh"

#define INITIAL_WINDOW_SIZE_X 1280
#define INITIAL_WINDOW_SIZE_Y 720

class Engine
{
public:
	Engine()	= default;
	~Engine() = default;

	void Initialize();
	void Run();
	void ShutDown();

	Editor editor;
	FrameBuffer framebuffer;

private:
	void InitOpenGL();
	void LoadShaders();
};
