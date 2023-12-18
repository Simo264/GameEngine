#pragma once

#include "../Core.hh"

// Load and retrieve font files located within the Fonts directory
// ------------------------------------------------------------------------
class FontsManager
{
public:
	FontsManager() = delete;
	~FontsManager() = delete;

	static void Init();

	static std::filesystem::path GetFont(std::filesystem::path filePath);

private:
	static std::filesystem::path _fontsDir;
};