#pragma once

#include "Core/Core.hpp"

class Engine
{
public:
	Engine()	= default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

	class Editor* editor;
};
