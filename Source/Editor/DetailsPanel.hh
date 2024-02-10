#pragma once

#include "../SceneObject.hh"

class DetailsPanel
{
public:
	DetailsPanel(const char* panelName, Vec2i panelSize);
	~DetailsPanel() = default;

	bool isOpen;

	template<class T>
	void RenderPanel(SceneObject<T>& sceneObject);

private:
	String _panelName;
	Vec2i _panelSize;
};

