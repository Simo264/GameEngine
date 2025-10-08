#pragma once

#include "Types.hpp"

class Entity
{
public:
	constexpr Entity() : _id{ INVALID_ENTITY_ID } {}
	constexpr Entity(EntityId id) : _id{ id }{}
	constexpr EntityId Id() const { return _id;	}
	constexpr bool Valid() const { return _id != INVALID_ENTITY_ID; }
	constexpr bool operator==(const Entity& other) const { return _id == other.Id(); }
	constexpr bool operator!=(const Entity& other) const { return !(*this == other); }

private:
	EntityId _id;
};