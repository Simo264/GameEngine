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
		assert(newValue.size() < 32);

		value.fill(0);
		std::strncpy(value.data(), newValue.data(), newValue.size());
	}

	Array<char, 32> value;
};