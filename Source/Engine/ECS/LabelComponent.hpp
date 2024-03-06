#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/IComponent.hpp"

class LabelComponent : public IComponent
{
public:
	LabelComponent(const char* label) 
		: label{ label } 
	{}

	static const char* GetComponentName(bool lower = false)
	{
		if(lower)
			return "labelcomponent";

		return "LabelComponent";
	}

	/* Return string "label=<label>" */
	void ToString(String& out) const override
	{
		out.append(label.c_str());
	}

	String label;
};