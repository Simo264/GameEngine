#pragma once

#include "Editor/Panel.hpp"
#include "Core/Math/Math.hpp"

/* ----------------------------------------
			DetailsPanel class
	---------------------------------------- */
class DetailsPanel : public Panel
{
	friend class Editor;

public:
	DetailsPanel(const char* panelName, bool visible);

	void RenderPanel(class GameObject* selected);

private:
	int _grizmoMode;

	template<int cols>
	void CreateTable(const char* label, Array<float, cols> sizes);
	void EndTable();
	void EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize);

	void LightProperties(class LightComponent*);
};

