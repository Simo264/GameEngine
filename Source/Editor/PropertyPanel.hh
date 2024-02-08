#pragma once

#include "../SceneObject.hh"

class PropertyPanel
{
public:
	PropertyPanel(const char* panelName, Vec2i panelSize);
	~PropertyPanel() = default;

	bool isOpen;

	void RenderPanel(SceneObject<class DirectionalLight>& sceneDirLight);
	void RenderPanel(SceneObject<class PointLight>& scenePointLight);
	void RenderPanel(SceneObject<class StaticMesh>& sceneStaticMesh);

private:
	String _panelName;
	Vec2i _panelSize;
	Vector<class Texture2D*> _textures;
};

