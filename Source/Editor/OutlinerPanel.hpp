#pragma once

#include "Core/Core.hpp"
#include "Editor/Panel.hpp"
#include "Engine/GameObject.hpp"

/* ----------------------------------------
			OutlinerPanel class
	---------------------------------------- */
class OutlinerPanel : public Panel
{
public:
	OutlinerPanel(const char* panelName, bool visible);

	void RenderPanel(class Scene* scene);
	bool IsItemSelected() const { return false; }

	GameObject& GetItemSelected() { return _selected; }

private:
	const float _iconSize;
	GameObject _selected; /* entity id */

	class Texture2D* GetObjectIcon(uint32_t objectType);
	void SetIconTooltip(uint32_t objectType);

	enum class ICON_TYPE : int {
		DIR_LIGHT = 0,
		POINT_LIGHT,
		SPOT_LIGHT,

		STATIC_MESH,

		VISIBLE,
		HIDDEN,

		COUNT
	};
	
	Array<Texture2D*, static_cast<int>(ICON_TYPE::COUNT)> _icons;

#if 0
	void ToggleVisibility(SharedPointer<DirectionalLight>& dLight);
	void ToggleVisibility(SharedPointer<PointLight>& pLight);
	void ToggleVisibility(SharedPointer<StaticMesh>& sMesh);
#endif

	void AddSceneComponentButton(const char* labelPopup);
	void AddSceneComponentPopup(class Scene* scene);
};

