#pragma once

#include "Core/Core.hpp"
#include "Core/Log/Logger.hpp"

#define MINI_CASE_SENSITIVE
#include <mini/ini.h>

class IniFileHandler
{
public:
	IniFileHandler(const fs::path& filePath)
	{
		// Create a file instance
		_file = std::make_unique<mINI::INIFile>(filePath.string());
	}
	~IniFileHandler() = default;

	/** @brief Return reference of ini data structure */
	mINI::INIStructure& GetData()
	{
		return _iniData;
	}

	/** @brief Read data from file */
	void ReadData()
	{
		_file->read(_iniData);
	}

	bool HasKey(StringView section, StringView key)
	{
		return _iniData.get(section.data()).has(key.data());
	}

	/** @brief Return a reference to real data and creates a new item automatically if one does not already exist */
	const String& GetValue(StringView section, StringView key)
	{
		static String empty{};
		if (!HasKey(section, key))
			return empty;

		return _iniData[section.data()][key.data()];
	}

	/** @brief To update a value */
	void Update(StringView section, StringView key, StringView value)
	{
		_iniData[section.data()][key.data()] = value.data();
	}

	/** @brief Set multiple values at once */
	void Set(StringView section, const Vector<std::pair<String, String>>& args)
	{
		_iniData[section.data()].set(args);
	}

	/** @brief To remove a single key from a section */
	bool RemoveKey(StringView section, StringView key)
	{
		return _iniData[section.data()].remove(key.data());
	}

	/** @brief To remove a section */
	bool RemoveSection(StringView section)
	{
		_iniData.remove(section.data());
	}

	/** @brief To remove all keys from a section */
	void ClearSection(StringView section)
	{
		_iniData[section.data()].clear();
	}

	/** @brief To remove all data in structure */
	void Clear()
	{
		_iniData.clear();
	}

	/** @brief To check if a section is present */
	bool HasSection(StringView section)
	{
		return _iniData.has(section.data());
	}

	/** @brief To write back to a file while preserving comments and custom formatting */
	bool Write(bool prettyprint = false)
	{
		return _file->write(_iniData, prettyprint);
	}

	/** @brief To generate a file. Note that generate() will overwrite any custom formatting and comments from the original file! */
	void Generate(bool prettyprint = false)
	{
		_file->generate(_iniData, prettyprint);
	}

private:
	UniquePtr<mINI::INIFile> _file;
	mINI::INIStructure _iniData;
};