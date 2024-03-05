#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/IComponent.hpp"

class TypeComponent : public IComponent
{
public:
	TypeComponent(uint32_t type)
		: type{ type }
	{}

	/* Return following string representation: "type=<type>" */
	void ToString(String& out) const override
	{
		char buff[32]{};
		sprintf_s(buff, "type=%d\n", type);

		out.append(buff);
	}

	uint32_t type;
};

