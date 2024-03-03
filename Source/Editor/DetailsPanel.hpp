#pragma once

#include "Editor/Panel.hpp"
#include "Core/Math/Math.hpp"

/* ----------------------------------------
			DetailsPanel class
	---------------------------------------- */
class DetailsPanel : public Panel
{
public:
	DetailsPanel(const char* panelName, bool visible) 
		: Panel(panelName, visible)
	{}


#if 0
	void RenderPanel(SharedPointer<DirectionalLight>& target, class Scene*);
	void RenderPanel(SharedPointer<PointLight>& target, class Scene*);
	void RenderPanel(SharedPointer<StaticMesh>& target, class Scene*);
#endif

private:
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);
};

