#pragma once

#include "Core/Core.hpp"

#include <mini/ini.h>

class INIParser
{
public:
	INIParser(const fs::path& filePath);
	~INIParser() = default;

	/** @brief Return reference of ini data structure */
	mINI::INIStructure& GetData();

	/** @brief Read data from file */
	void ReadData();

	bool HasKey(StringView section, StringView key);

	/** @brief Return a reference to real data and creates a new item automatically if one does not already exist */
	const String& GetValue(StringView section, StringView key);

	/** @brief To update a value */
	void Update(StringView section, StringView key, StringView value);

	/** @brief Set multiple values at once */
	void Set(StringView section, const Vector<std::pair<String, String>>& args);

	/** @brief To remove a single key from a section */
	bool RemoveKey(StringView section, StringView key);

	/** @brief To remove a section */
	bool RemoveSection(StringView section);

	/** @brief To remove all keys from a section */
	void ClearSection(StringView section);

	/** @brief To remove all data in structure */
	void Clear();

	/** @brief To check if a section is present */
	bool HasSection(StringView section);

	/** @brief To write back to a file while preserving comments and custom formatting */
	bool Write(bool prettyprint = false);

	/** @brief To generate a file. Note that generate() will overwrite any custom formatting and comments from the original file! */
	void Generate(bool prettyprint = false);

private:
	UniquePtr<mINI::INIFile> _file;
	mINI::INIStructure _iniData;
};