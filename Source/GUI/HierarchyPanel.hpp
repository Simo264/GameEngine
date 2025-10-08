#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/Math/Types.hpp"
#include "Engine/ECS/Entity.hpp"
#include "PanelBase.hpp"

class HierarchyPanel : public PanelBase
{
public:
  HierarchyPanel() : 
    isOpen{ true },
    target{}
  {}

	void Render();

	bool isOpen;
  Entity target;

private:
  bool __ButtonCentered(StringView label, Vec2I size);
  void __RenderNewEntityModal();
  void __RenderEntityList();
};
