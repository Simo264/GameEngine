#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/Scene.hpp"

class HierarchyPanel
{
public:
  HierarchyPanel() : isOpen{ true }
  {}

	void Render(Scene& scene);

	bool isOpen;
  Entity selectedEntity{ INVALID_ENTITY_ID, nullptr, nullptr };

private:
  struct EntityCreationData
  {
    Array<char, 32> tag{ "entity" };
    bool tagIsValid{ false };

    const EntityArchetype* selectedArchetype{ nullptr };
    ArchetypeId selectedArchetypeId{ INVALID_ARCHETYPE_ID };

    LightType lightType{ LightType::None };
    fs::path modelPath{};
  };
  EntityCreationData _entityData;

  bool __ButtonCentered(StringView label, Vec2I size);
  void __RenderNewEntityModal(Scene& scene);
  void __RenderEntityList(Scene& scene);
};
