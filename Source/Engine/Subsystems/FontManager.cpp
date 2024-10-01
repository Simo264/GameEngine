#include "FontManager.hpp"

#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------  */
/*                    PUBLIC                              */
/* -----------------------------------------------------  */

void FontManager::Initialize()
{
	CONSOLE_INFO("Loading fonts...");
	LoadFonts();
}

Map<String, fs::path>::iterator FontManager::GetRecordByName(StringView fontName)
{
	const auto& it = _fonts.find(fontName.data());
	if (it == _fonts.end())
		throw std::runtime_error(std::format("Fonts '{}' does not exist", fontName.data()));

	return it;
}

/* -----------------------------------------------------  */
/*                    PRIVATE                             */
/* -----------------------------------------------------  */

void FontManager::LoadFonts()
{
	for (auto& entry : fs::recursive_directory_iterator(GetFontsPath()))
	{
		if (fs::is_directory(entry))
			continue;

		const fs::path& entryPath = entry.path();
		const String filename = entryPath.filename().string();
		auto res = _fonts.emplace(filename.c_str(), entryPath);
		if (!res.second)
			CONSOLE_WARN("Error on loading font <{}>", filename);
	}
}
