#pragma once

#include "ComponentMacros.hpp"

namespace Components
{
	DECLARE_COMPONENT(Tag)
	{
		static constexpr u64 MAX_TAG_LENGTH = 32;
		using TagValue = Array<char, MAX_TAG_LENGTH>;

		Tag() : value{} {}
		Tag(StringView newValue)
		{
			Update(newValue);
		}

		void Update(StringView newValue)
		{
			value = TagValue{};
			auto n = std::min(MAX_TAG_LENGTH - 1, newValue.size());
			std::copy_n(newValue.begin(), n, value.begin());
			value[n] = '\0';
		}

		TagValue value;
	};
}
