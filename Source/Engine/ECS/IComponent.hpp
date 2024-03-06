#pragma once

#include "Core/Core.hpp"

class IComponent
{
public:
	IComponent() = default;
	~IComponent() = default;

	/* For ech attribute in component return the following string: 
		"<attr1>=<value1>"
		"<attr2>=<value2>"
		...
	*/
	virtual void ToString(String& out) const = 0;
};
