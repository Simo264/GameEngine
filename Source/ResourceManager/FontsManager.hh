#pragma once

#include "../Core.hh"

class FontsManager
{
public:
	FontsManager() = delete;
	~FontsManager() = delete;

	static void Init();

	static std::filesystem::path GetFontByName(const char* filename);

private:
	static std::filesystem::path _fontsDir;
};