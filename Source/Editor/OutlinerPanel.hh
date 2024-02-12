#pragma once

#include "../SceneObject.hh"
#include "../Scene.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"

/* ----------------------------------------
			OutlinerPanel class
	---------------------------------------- */
class OutlinerPanel
{
public:
	OutlinerPanel(const char* panelName);
	~OutlinerPanel() = default;

	bool isOpen;
	String panelName;

	void RenderPanel(Scene* scene);
	bool IsItemSelected() const { return _dLightSelected || _pLightSelected || _sMeshSelected; }

	template<class T>
	SceneObject<T>* GetItemSelected();

private:
	enum ICONS : int { PLUS = 0, SUN, LAMP, EYE, EYE_HIDDEN, MESH };

	Array<class Texture2D*, 6> _icons;
	float _iconSize;
	uint32_t _buttonEyeID;

	SceneObject<DirectionalLight>* _dLightSelected;
	SceneObject<PointLight>* _pLightSelected;
	SceneObject<StaticMesh>* _sMeshSelected;
	
	void ToggleVisibility(SceneObject<DirectionalLight>& sceneObj);
	void ToggleVisibility(SceneObject<PointLight>& sceneObj);
	void ToggleVisibility(SceneObject<StaticMesh>& sceneObj);
	
	
	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(Scene* scene);
};

