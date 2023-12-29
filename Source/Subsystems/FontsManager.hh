#pragma once

#include "../Core.hh"

// Load and retrieve font files located within the Fonts directory
// ------------------------------------------------------------------------
class FontsManager
{
public:
	FontsManager()  = delete;
	~FontsManager() = delete;

	static std::filesystem::path GetFont(std::filesystem::path filePath);
};