#pragma once

#include "Core.hpp"

#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"
#include "Engine/StaticMesh.hpp"

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
	
	void RenderPanel(SharedPointer<DirectionalLight>& target, class Scene*);
	void RenderPanel(SharedPointer<PointLight>& target, class Scene*);
	void RenderPanel(SharedPointer<StaticMesh>& target, class Scene*);

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);
};

