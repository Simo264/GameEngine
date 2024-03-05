#pragma once

#include "Core/Core.hpp"

class IComponent
{
public:
	IComponent() = default;
	~IComponent() = default;

	/* Return string component in INI format */
	virtual void ToString(String& out) const = 0;
};
