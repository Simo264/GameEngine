#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"

class Label
{
public:
	Label(StringView label) 
	{
		UpdateValue(label);
	}
	
	void UpdateValue(StringView label)
	{
		if (label.empty())
		{
			std::fill_n(value, sizeof(value), 0);
			return;
		}
		
		if (label.size() >= 64)
			CONSOLE_WARN("Label.size() >= 64");

		std::fill_n(value, sizeof(value), 0);
		std::copy_n(label.data(), sizeof(value), value);
	}

	~Label() = default;

	char value[64]{};
};