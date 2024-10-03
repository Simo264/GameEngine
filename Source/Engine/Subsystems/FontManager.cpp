#include "FontManager.hpp"

#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------  */
/*                    PUBLIC                              */
/* -----------------------------------------------------  */

void FontManager::Initialize()
{
	CONSOLE_INFO("Loading fonts...");
	
	for (auto& entry : fs::recursive_directory_iterator(GetFontsPath()))
	{
		if (fs::is_directory(entry))
			continue;

		const fs::path& entryPath = entry.path();
		const String filename = entryPath.filename().string();
		auto res = _fonts.emplace(filename.c_str(), entryPath);
		CONSOLE_TRACE("{}", filename);

		if (!res.second)
			CONSOLE_ERROR("Error on loading font");
	}
}

Map<String, fs::path>::iterator FontManager::GetRecordByName(StringView fontName)
{
	const auto& it = _fonts.find(fontName.data());
	assert(it != _fonts.end() && "Font does not exist");
	return it;
}

/* -----------------------------------------------------  */
/*                    PRIVATE                             */
/* -----------------------------------------------------  */

