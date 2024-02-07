#pragma once

#include "../SceneObject.hh"
#include "../Scene.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"

class ScenePanel
{
public:
	ScenePanel(const char* panelName, Vec2i panelSize);
	~ScenePanel() = default;

	bool isOpen;
	bool isPropertiesOpen;

	void RenderPanel(Scene* scene);

private:
	Vector<class Texture2D*> _textures;
	class Texture2D* _iconPlus;
	class Texture2D* _iconSun;
	class Texture2D* _iconLamp;
	class Texture2D* _iconEye;
	class Texture2D* _iconEyeHidden;
	class Texture2D* _iconMesh;

	bool _dirLightSelected;
	int _pointLightSelected;
	int _staticMeshSelected;

	float _iconSize;
	
	String _panelName;
	Vec2i _panelSize;
	
	void ShowPropertiesPanel(Scene* scene, SceneObject<DirectionalLight>* sceneDirLight);
	void ShowPropertiesPanel(SceneObject<PointLight>* scenePointLight);
	void ShowPropertiesPanel(SceneObject<StaticMesh>* sceneMesh);

	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(Scene* scene);

	/* Show directional light row of the scene in the table */
	void DirLightRow(SceneObject<DirectionalLight>* sceneDirLight);

	/* List the point light of the scene in the table */
	void PointLightRow(const char* label, int i);

	/* List the static mesh of the scene in the table */
	void StaticMeshRow(const char* label, int i);
};

