#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/Math/Types.hpp"
#include "PanelBase.hpp"

namespace Components
{
  struct Transform;
};
struct Texture2D;
class Entity;

class ViewportPanel : public PanelBase
{
public:
  ViewportPanel() : 
    isOpen{ true },
    isFocused{ false },
    size{},
    position{},
    gizmoOp{ -1 }
  {}
  
  void Render(StringView windowName, 
              Texture2D viewportImage,
              Entity target,
              const Mat4F& view, 
              const Mat4F& proj);
  
  bool isOpen;
  bool isFocused;
  Vec2I size;
  Vec2I position;
  i32 gizmoOp;

private:
  void __GizmoWorldTranslation(Components::Transform& transform);
  void __GizmoWorldRotation(Components::Transform& transform);
  void __GizmoWorldScaling(Components::Transform& transform);
};
