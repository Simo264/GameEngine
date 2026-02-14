#pragma once

#include "Core/Strings.hpp"
#include "Engine/ECS/Entity.hpp"

class EntityBuilder
{
public:
  virtual ~EntityBuilder() = default;

  virtual void WithArchetypeIdentifier(ArchetypeId archetype) = 0;
  virtual void WithTag(StringView tag) = 0;
  virtual Entity Build() = 0;
};
