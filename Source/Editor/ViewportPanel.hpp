#pragma once

#include "Editor/Panel.hpp"
#include "Core/Math/Math.hpp"

/* ----------------------------------------
			Viewport class 
	---------------------------------------- */
class ViewportPanel : public Panel
{
	friend class Editor;

public:
	ViewportPanel(const char* panelName, bool visible);
	
	bool isFocused;
	Vec2i32 viewportSize;

	/**
	 * .
	 * 
	 * \param camera: used in Grizmo transformations
	 * \param framebuffer: draw the framebuffer image 
	 * \param selected: used in Grizmo transformations
	 */
	void RenderPanel(
		class Camera& camera, 
		const uint32_t framebufferImage, 
		class GameObject& selected);

private:
	int _grizmoMode;

	void GrizmoTransformation(
		class Camera& camera,
		struct TransformComponent& component);
};
