#pragma once

#include "../Core.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Mesh/StaticMesh.hh"

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
	
	void RenderPanel(SharedPointer<DirectionalLight>& target, class Scene*);
	void RenderPanel(SharedPointer<PointLight>& target, class Scene*);
	void RenderPanel(SharedPointer<StaticMesh>& target, class Scene*);

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);
};

