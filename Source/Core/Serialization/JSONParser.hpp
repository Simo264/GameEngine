#pragma once

#include "Core/Core.hpp"
#include <nlohmann/json.hpp>

namespace JSONParser
{
	nlohmann::json ParseFile(const fs::path& path);
};