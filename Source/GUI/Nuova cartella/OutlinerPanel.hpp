//#pragma once
//
//#include "Core/Core.hpp"
//#include "Editor/Panel.hpp"
//#include "Engine/GameObject.hpp"
//
//class OutlinerPanel : public Panel
//{
//	friend class Editor;
//
//public:
//	OutlinerPanel(const char* panelName, bool visible);
//
//	void RenderPanel(class Scene& scene) {};
//	bool IsItemSelected() const { return false; }
//
//	/* The selected object can be invalid */
//	GameObject& GetItemSelected() { return _selected; }
//
//private:
//	const float _iconSize;
//	GameObject _selected;
//
//	//const char* GetTypeName(GameObjectType objectType);
//
//	void AddSceneComponentButton(const char* labelPopup);
//	void AddSceneComponentPopup(class Scene& scene);
//};
//
