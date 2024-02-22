#pragma once

#include "Core.hpp"

/* ----------------------------------------
			InspectorPanel class
	---------------------------------------- */
class InspectorPanel
{
public:
	InspectorPanel(const char* panelName);

	bool isOpen;
	String panelName;

	void RenderPanel();
};
