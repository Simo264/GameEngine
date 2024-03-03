#pragma once

#include "Core/Core.hpp"

class Panel
{
public:
	Panel(const char* panelName, bool visible)
		: panelName{panelName},
			visible{ visible }
	{}

	String panelName;
	bool visible;
};

