#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"

/**
 * Tags provide a way of marking and identifying types of GameObject in your scene
 */
class Tag
{
public:
	Tag(StringView tag) 
	{
		UpdateValue(tag);
	}
	
	void UpdateValue(StringView newValue)
	{
		if (newValue.empty())
		{
			std::fill_n(value, sizeof(value), 0);
			return;
		}
		if (newValue.size() >= 64)
		{
			CONSOLE_WARN("Label.size() >= 64");
			return;
		}

		std::fill_n(value, sizeof(value), 0);
		std::copy_n(newValue.data(), newValue.size(), value);
	}

	char value[64]{};
};