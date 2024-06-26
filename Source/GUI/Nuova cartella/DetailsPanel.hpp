//#pragma once
//
//#include "Editor/Panel.hpp"
//#include "Core/Math/Math.hpp"
//
///* ----------------------------------------
//			DetailsPanel class
//	---------------------------------------- */
//class DetailsPanel : public Panel
//{
//	friend class Editor;
//
//public:
//	DetailsPanel(const char* panelName, bool visible);
//
//	void RenderPanel(class Scene& scene, class GameObject& selected) {};
//
//private:
//	int _grizmoMode;
//
//	template<int cols>
//	void CreateTable(const char* label, array<float, cols> sizes);
//	void EndTable();
//	void EditVec3(const char* label, float speed, vec2f minMax, vec3f& values, float colSize);
//
//	void LightProperties(class LightComponent*);
//};
//
