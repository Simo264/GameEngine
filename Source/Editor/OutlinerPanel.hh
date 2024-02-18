#pragma once

#include "../Core.hh"

#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"

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

	void RenderPanel(class Scene* scene);
	bool IsItemSelected() const { return _dLightSelected || _pLightSelected || _sMeshSelected; }

	template<class T>
	SharedPointer<T>& GetItemSelected();

private:
	enum ICONS : int { PLUS = 0, SUN, LAMP, EYE, EYE_HIDDEN, MESH };

	Array<class Texture2D*, 6> _icons;
	float _iconSize;
	uint32_t _buttonEyeID;

	SharedPointer<DirectionalLight> _dLightSelected;
	SharedPointer<PointLight> _pLightSelected;
	SharedPointer<StaticMesh> _sMeshSelected;

	void ToggleVisibility(SharedPointer<DirectionalLight>& dLight);
	void ToggleVisibility(SharedPointer<PointLight>& pLight);
	void ToggleVisibility(SharedPointer<StaticMesh>& sMesh);
	
	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(class Scene* scene);
};

