#include "Tag.hpp"
#include "Core/Logger.hpp"

namespace Components
{
	void Tag::UpdateValue(StringView newTag)
	{
		if (newTag.size() >= 32)
			CONSOLE_WARN("tag value size >= 32");

		value.fill(0);
		std::copy_n(newTag.begin(), std::min(newTag.size(), value.size() - 1), value.begin());
	}
}
