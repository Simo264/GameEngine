#pragma once

#include "../Core.hh"

// Static class FontsManager: 
// load and retrieve font files located within the Fonts directory
// ---------------------------------------------------------------
class FontsManager
{
public:
	FontsManager()  = delete;
	~FontsManager() = delete;

	static Path GetFont(Path filePath);

private:
	inline static const Path _fontsDir = std::filesystem::current_path().parent_path() / "Fonts";
};