#pragma once

#include "../Core.hh"

/* ----------------------------------------
			DetailsPanel class
	---------------------------------------- */
class DetailsPanel
{
public:
	DetailsPanel(const char* panelName) : 
		panelName{ panelName },
		isOpen{ true }
	{}
	~DetailsPanel() = default;

	bool isOpen;
	String panelName;
	
	template<class T>
	void RenderPanel(class SceneObject* sceneObject);

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);
};

