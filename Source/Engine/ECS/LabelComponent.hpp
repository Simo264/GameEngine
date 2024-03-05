#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/IComponent.hpp"

class LabelComponent : public IComponent
{
public:
	LabelComponent(const char* label) 
		: label{ label } 
	{}

	/* Return following string representation: "label=<label>" */
	void ToString(String& out) const override
	{
		char buff[64]{};
		sprintf_s(buff, "label=%s\n", label.c_str());
		out.append(buff);
	}

	String label;
};