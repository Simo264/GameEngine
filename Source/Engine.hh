#pragma once

#include "Core.hh"
#include "Editor/Editor.hh"

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
