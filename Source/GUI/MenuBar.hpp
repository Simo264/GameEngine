#pragma once

#include "Core/Core.hpp"

class Scene;

class MenuBar
{
public:
	MenuBar(){}

	void Render(Scene& scene);
private:
	void __RenderFileMenu(Scene& scene);
	void __RenderViewMenu();
};

