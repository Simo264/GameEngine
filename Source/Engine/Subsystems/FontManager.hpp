#pragma once

#include "Core/Core.hpp"

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

	/**
	 * Load all fonts from default directory
	 */
	void Initialize();

	/**
	 * @return
	 * A iterator to the mapped value of the element with specified key.
	 * 
	 * @throw 
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	auto GetRecordByName(StringView fontName) { return _fonts.find(fontName.data()); }

	const auto& GetFonts() const { return _fonts; }

private:
	FontManager() = default;
	~FontManager() = default;

	/**
	 * First: the font name
	 * Second: the path of the font file
	 */
	Map<String, fs::path> _fonts;
};
