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

	Map<String, fs::path>::iterator GetRecordByName(StringView fontName);

	/**
	 * Load all fonts from default directory
	 */
	void Initialize();

	constexpr const Map<String, fs::path>& GetFonts() const { return _fonts; }

private:
	FontManager() = default;
	~FontManager() = default;

	/**
	 * First: the font name
	 * Second: the path of the font file
	 */
	Map<String, fs::path> _fonts;
};
