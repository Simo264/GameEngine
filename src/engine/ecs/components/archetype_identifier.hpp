#pragma once

#include "ComponentMacros.hpp"
#include "Engine/ECS/Types.hpp"

namespace Components
{
	DECLARE_COMPONENT(ArchetypeIdentifier)
	{
		ArchetypeIdentifier() : archetypeId{ INVALID_ARCHETYPE_ID } {}
		ArchetypeIdentifier(ArchetypeId id) : archetypeId{ id } {}
		ArchetypeId archetypeId;
	};
}