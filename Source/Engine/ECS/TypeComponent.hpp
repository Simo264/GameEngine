#pragma once

#pragma once

#include "Core/Core.hpp"
#include "Engine/GameObjectType.hpp"

class TypeComponent
{
public:
	TypeComponent(GameObjectType type)
		: type{ type }
	{}

	GameObjectType type;
};