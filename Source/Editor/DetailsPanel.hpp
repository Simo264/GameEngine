#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

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

	bool isOpen;
	String panelName;

#if 0
	void RenderPanel(SharedPointer<DirectionalLight>& target, class Scene*);
	void RenderPanel(SharedPointer<PointLight>& target, class Scene*);
	void RenderPanel(SharedPointer<StaticMesh>& target, class Scene*);
#endif

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);
};

