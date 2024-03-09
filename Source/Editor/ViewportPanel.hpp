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
	Vec2i viewportSize;

	void RenderPanel(class Scene& scene, class FrameBuffer& framebuffer, class GameObject* selected);

private:
	int _grizmoMode;

	void GrizmoTransformation(class Scene& scene, struct TransformComponent& component);
};
