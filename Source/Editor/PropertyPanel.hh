#pragma once

#include "../SceneObject.hh"

class PropertyPanel
{
public:
	PropertyPanel(const char* panelName, Vec2i panelSize);
	~PropertyPanel() = default;

	bool isOpen;

	template<class T>
	void RenderPanel(SceneObject<T>& sceneObject);

private:
	String _panelName;
	Vec2i _panelSize;
};

