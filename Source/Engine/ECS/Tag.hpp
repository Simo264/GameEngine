#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"

/**
 * @brief
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
		std::fill_n(value, sizeof(value), 0);

		if (newValue.empty())
			return;
		
		if (newValue.size() >= 64)
		{
			CONSOLE_WARN("newValue.size() >= 64");
			return;
		}

		std::copy_n(newValue.data(), newValue.size(), value);
	}

	char value[64]{};
};