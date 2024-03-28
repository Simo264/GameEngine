#pragma once

#include "Core/Core.hpp"
#include "Editor/Editor.hpp"

class Engine
{
public:
	Engine()	= default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

	Editor editor;

private:
	void SetGLStates();

	void LoadConfig();
	
	/* Load shader programs from Shaders/ directory */
	void LoadShaders();
	
	/* Load textures from Textures/ directory */
	void LoadTextures();

	/* Load textures from Icons/ directory */
	void LoadIcons();
};
