#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include <mini/ini.h>

/**
 * Read and load data from .ini file
 */
class INIFileParser{
public:
	INIFileParser(const Path& filepath);

	void PrintContent();

	/* Return reference of ini data structure */
	mINI::INIStructure& GetData() { return _iniData; }

	/* Read data from file */
	void ReadData()
	{
		_file->read(_iniData);
	}

	/* Returns a copy of the data and doesn't create new items in the structure */
	String GetValue(const char* section, const char* key)
	{
		return _iniData.get(section).get(key);
	}

	/* To update a value */
	void Update(const char* section, const char* key, const char* value)
	{
		_iniData[section][key] = value;
	}

	/* Set multiple values at once */
	void Set(const char* section, const Vector<std::pair<String, String>>& args)
	{
		_iniData[section].set(args);
	}

	/* To remove a single key from a section */
	bool RemoveKey(const char* section, const char* key)
	{
		return _iniData[section].remove(key);
	}

	/* To remove a section */
	bool RemoveSection(const char* section)
	{
		_iniData.remove(section);
	}

	/* To remove all keys from a section */
	void ClearSection(const char* section)
	{
		_iniData[section].clear();
	}

	/* To remove all data in structure */
	void Clear()
	{
		_iniData.clear();
	}

	/* To check if a section is present */
	bool HasSection(const char* section)
	{
		return _iniData.has(section);
	}
	
	/* To write back to a file while preserving comments and custom formatting */
	bool Write(bool prettyprint=false)
	{
		return _file->write(_iniData, prettyprint);
	}

	/* To generate a file. Note that generate() will overwrite any custom formatting and comments from the original file! */
	void Generate(bool prettyprint = false)
	{
		_file->generate(_iniData, prettyprint);
	}

	/* From string "50,50" -> to Vec2i(50,50) */
	static Vec2i32 StringToVec2i(String& str, const char* delimiter = ",");
	static Vec2f StringToVec2f(String& str, const char* delimiter = ",");
	
	/* From string "50,50,50" -> to Vec3i(50,50,50) */
	static Vec3i32 StringToVec3i(String& str, const char* delimiter = ",");
	static Vec3f StringToVec3f(String& str, const char* delimiter = ",");

	/* Parse strings { "1" "0" "true" "false" } to boolean */
	static bool StringToBool(const String& str);

private:
	UniquePointer<mINI::INIFile> _file;
	mINI::INIStructure _iniData;
};

