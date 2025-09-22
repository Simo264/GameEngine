#pragma once

#include "Core/Core.hpp"

using ArchetypeId = u32;
constexpr ArchetypeId INVALID_ARCHETYPE_ID = static_cast<ArchetypeId>(-1);

namespace Components
{
	struct ArchetypeIdentifier
	{
		ArchetypeIdentifier() : archetypeId{ INVALID_ARCHETYPE_ID } {}
		explicit ArchetypeIdentifier(ArchetypeId id) : archetypeId{ id } {}
		
		ArchetypeId archetypeId{ INVALID_ARCHETYPE_ID };
	};
}