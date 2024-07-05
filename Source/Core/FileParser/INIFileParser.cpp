#include "INIFileParser.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

INIFileParser::INIFileParser(const fspath& filepath)
{
	if (!fs::exists(filepath) || !fs::is_regular_file(filepath))
	{
		CONSOLE_WARN("<filepath> is not a valid path");
		return;
	}

	/* Create a file instance */
	_file = std::make_unique<mINI::INIFile>(filepath.string());
}
