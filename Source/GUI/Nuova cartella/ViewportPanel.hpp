//#pragma once
//
//#include "Editor/Panel.hpp"
//#include "Core/Math/Math.hpp"
//
///* ----------------------------------------
//			Viewport class 
//	---------------------------------------- */
//class ViewportPanel : public Panel
//{
//	friend class Editor;
//
//public:
//	ViewportPanel(const char* panelName, bool visible);
//	
//	bool isFocused;
//	vec2i32 viewportSize;
//
//	void RenderPanel(
//		class Camera& camera, 
//		const uint32_t framebufferImage, 
//		class GameObject& selected);
//
//private:
//	int _grizmoMode;
//
//	void GrizmoTransformation(
//		class Camera& camera,
//		struct TransformComponent& component);
//};