#pragma once

#include "Core.hpp"
#include "Editor/Editor.hpp"

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
	
	/* Load shader programs from Shaders/ directory */
	void LoadShaders();
	
	/* Load textures from Textures/ directory */
	void LoadTextures();

	/* Load textures from Icons/ directory */
	void LoadIcons();
};
