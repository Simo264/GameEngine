#include "ConfigFile.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ConfigFile::ConfigFile(const fs::path& filepath)
{
	if (!fs::exists(filepath) || !fs::is_regular_file(filepath))
	{
		CONSOLE_WARN("<filepath> is not a valid path");
		return;
	}

	/* Create a file instance */
	_file = std::make_unique<mINI::INIFile>(filepath.string());
}