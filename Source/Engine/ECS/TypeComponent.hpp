#pragma once

#include "Core/Core.hpp"

class TypeComponent
{
public:
	TypeComponent(uint32_t type)
		: type{ type }
	{}

	uint32_t type;
};

