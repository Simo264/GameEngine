#pragma once

#include "Core/Core.hpp"

/**
 * Load and retrieve texture objects
 */
class FontManager
{
public:
	FontManager(const FontManager&) = delete;
	void operator=(const FontManager&) = delete;

	/**
	 * Return the instance of this FontManager singleton class
	 */
	static FontManager& Get()
	{
		static FontManager fontManager;
		return fontManager;
	}

	const fs::path& GetFontByName(StringView fontName);

	/**
	 * Load all fonts from default directory
	 */
	void Initialize();

	constexpr const UnorderedMap<String, fs::path>& GetFonts() const { return _fonts; }

private:
	FontManager() = default;
	~FontManager() = default;

	void LoadFonts();
	
	/**
	 * First: the filename of the font
	 * Second: the path of the font
	 */
	UnorderedMap<String, fs::path> _fonts;
};
