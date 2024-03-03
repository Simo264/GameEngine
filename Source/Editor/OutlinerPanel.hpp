#pragma once

#include "Editor/Panel.hpp"
#include "Engine/GameObjectType.hpp"

/* ----------------------------------------
			OutlinerPanel class
	---------------------------------------- */
class OutlinerPanel : public Panel
{
public:
	OutlinerPanel(const char* panelName, bool visible)
		: Panel(panelName, visible),
		_iconSize{ 16.0f },
		_buttonEyeID{ 1 }
	{}

	void RenderPanel(class Scene* scene);
	bool IsItemSelected() const { return false; }

	//template<class T>
	//SharedPointer<T>& GetItemSelected();

private:
	float _iconSize;
	uint32_t _buttonEyeID;

	class Texture2D* GetObjectIcon(GameObjectType type);

#if 0
	SharedPointer<DirectionalLight> _dLightSelected;
	SharedPointer<PointLight> _pLightSelected;
	SharedPointer<StaticMesh> _sMeshSelected;
	void ToggleVisibility(SharedPointer<DirectionalLight>& dLight);
	void ToggleVisibility(SharedPointer<PointLight>& pLight);
	void ToggleVisibility(SharedPointer<StaticMesh>& sMesh);
#endif

	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(class Scene* scene);
};

