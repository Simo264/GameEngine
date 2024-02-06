#pragma once

#include "../Core.hh"

class ScenePanel
{
public:
	ScenePanel(const char* panelName, Vec2i panelSize);
	~ScenePanel() = default;

	bool isOpen;
	bool isPropertiesOpen;

	void RenderPanel(class Scene* scene);

private:
	Vector<class Texture2D*> _textures;
	class Texture2D* _iconSun;
	class Texture2D* _iconLamp;
	class Texture2D* _iconEye;
	class Texture2D* _iconEyeHidden;
	class Texture2D* _iconPlus;

	bool _dirLightSelected;
	int _pointLightSelected;
	int _staticMeshSelected;
	
	String _panelName;
	Vec2i _panelSize;
	
	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);

	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(class Scene* scene);

	/* Show directional light row of the scene in the table */
	void DirLightRow(const char* label);

	/* List the point light of the scene in the table */
	void PointLightRow(const char* label, int i);

	/* List the static mesh of the scene in the table */
	void StaticMeshRow(const char* label, int i);
};

