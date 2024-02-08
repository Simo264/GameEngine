#pragma once

#include "../SceneObject.hh"
#include "../Scene.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"

/* ----------------------------------------
			ScenePanel class
	---------------------------------------- */
class ScenePanel
{
public:
	ScenePanel(const char* panelName, Vec2i panelSize);
	~ScenePanel() = default;

	bool isOpen;

	void RenderPanel(Scene* scene);
	bool IsItemSelected() const { return _dLightSelected || _pLightSelected || _sMeshSelected; }
	SceneObject<DirectionalLight>* DirLightSelected() const { return _dLightSelected; }
	SceneObject<PointLight>* PointLightSelected() const { return _pLightSelected; }
	SceneObject<StaticMesh>* StaticMeshSelected() const { return _sMeshSelected; }

private:
	enum ICONS : int { PLUS = 0, SUN, LAMP, EYE, EYE_HIDDEN, MESH };
	Array<class Texture2D*, 6> _icons;
	float _iconSize;
	String _panelName;
	Vec2i _panelSize;
	uint32_t _buttonEyeID;

	SceneObject<DirectionalLight>* _dLightSelected;
	SceneObject<PointLight>* _pLightSelected;
	SceneObject<StaticMesh>* _sMeshSelected;
	
	void ToggleVisibility(bool& b);

#if 0
	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(Scene* scene);
#endif
};

