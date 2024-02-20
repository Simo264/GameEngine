#pragma once

#include "Core/Core.hpp"

/* ----------------------------------------
			InspectorPanel class
	---------------------------------------- */
class InspectorPanel
{
public:
	InspectorPanel(const char* panelName);
	~InspectorPanel() = default;

	bool isOpen;
	String panelName;

	void RenderPanel();
};
