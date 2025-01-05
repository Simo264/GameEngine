#pragma once

#include "Core/Core.hpp"

/**
 * @class FontManager
 * @brief Singleton class responsible for managing font resources.
 *
 * This class provides functionalities to load and access fonts from a specified directory.
 * It follows the singleton pattern to ensure only one instance of the manager exists throughout the application lifecycle.
 */
class FontsManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	FontsManager(const FontsManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const FontsManager&) = delete;

	/** @return Returns the singleton instance of FontManager. */
	static FontsManager& Get()
	{
		static FontsManager fontManager;
		return fontManager;
	}

	/**
	 * @brief Initializes the FontManager by loading all available fonts from the specified directory.
	 *
	 * This method scans the directory returned by GetFontsPath() and loads all font files recursively.
	 */
	void Initialize();
	
	/**
	 * @brief Retrieves the font record by its name.
	 * 
	 * @return Returns an iterator to the font record if found, otherwise returns _fonts.end().
	 */
	auto GetRecordByName(StringView fontName) { return _fonts.find(fontName.data()); }
	
	/** @return Returns a constant reference to the map of loaded fonts. */
	const auto& GetFonts() const { return _fonts; }

private:
	FontsManager() = default;
	~FontsManager() = default;

	/**
	 * @brief Map to store loaded font records.
	 *
	 * The key is the font name, and the value is the file path to the font.
	 */
	Map<String, fs::path> _fonts;
};
