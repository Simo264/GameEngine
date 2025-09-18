#pragma once

#include "Core/Core.hpp"

namespace Components
{
	struct Tag
	{
		Array<char, 32> value{};
		
		void UpdateValue(StringView newTag);
	};
}
