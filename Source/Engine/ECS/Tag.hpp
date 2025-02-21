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
		std::memset(value, 0, sizeof(value));

		if (newValue.empty())
			return;
		
		if (newValue.size() >= sizeof(value))
			CONSOLE_WARN("newValue.size() >= {}", sizeof(value));

		std::strncpy(value, newValue.data(), sizeof(value));
	}

	char value[32]{};
};