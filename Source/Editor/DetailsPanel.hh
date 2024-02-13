#pragma once

#include "../SceneObject.hh"

/* ----------------------------------------
			DetailsPanel class
	---------------------------------------- */
class DetailsPanel
{
public:
	DetailsPanel(const char* panelName);
	~DetailsPanel() = default;

	bool isOpen;
	String panelName;

	template<class T>
	void RenderPanel(SceneObject<T>& sceneObject);

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Array<float*, 3> coords, float colSize);
};

