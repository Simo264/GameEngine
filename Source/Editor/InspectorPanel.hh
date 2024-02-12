#pragma once

#include "../Core.hh"

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
