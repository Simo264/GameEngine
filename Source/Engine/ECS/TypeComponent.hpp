#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/IComponent.hpp"

class TypeComponent : public IComponent
{
public:
	TypeComponent(uint32_t type)
		: type{ type }
	{}

	static const char* GetComponentName(bool lower = false)
	{
		if(lower)
			return "typecomponent";

		return "TypeComponent";
	}

	/* Return "type=<type>" */
	void ToString(String& out) const override
	{
		char buff[32]{};
		sprintf_s(buff, "type=%d\n", type);
		out.append(buff);
	}

	uint32_t type;
};

