#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/Math/Types.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "PanelBase.hpp"

class ToolbarPanel : public PanelBase
{
public:
	ToolbarPanel();

	void Render(StringView windowName, Vec2I viewportPos, Vec2I viewportSize, i32& viewportGizmoOp);

  bool isOpen;

private:
	Texture2D _expandIcon;
	Texture2D _scaleIcon;
	Texture2D _rotateIcon;
	Texture2D _wireframeIcon;

  void __GizmoButton(StringView label,
                     Texture2D icon,
                     f32 btnSize,
                     i32& viewportGizmoOp,
                     i32 op,
                     StringView tooltip);

  void __WireframeButton(StringView label,
                         Texture2D icon,
                         f32 btnSize,
                         bool& wireframe,
                         StringView tooltip);
};

