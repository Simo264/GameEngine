#pragma once

#include "Core/Core.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"
#include "Engine/StaticMesh.hpp"

/* ----------------------------------------
			OutlinerPanel class
	---------------------------------------- */
class OutlinerPanel
{
public:
	OutlinerPanel(const char* panelName);
	
	bool isOpen;
	String panelName;

	void RenderPanel(class Scene* scene);
	bool IsItemSelected() const { return _dLightSelected || _pLightSelected || _sMeshSelected; }

	template<class T>
	SharedPointer<T>& GetItemSelected();

private:
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

