#pragma once

#include "PanelBase.hpp"

class MenuBar : public PanelBase
{
public:
	MenuBar() = default;

	void Render();
private:
	void __RenderFileMenu();
	void __RenderViewMenu();
};

