#include "INIParser.hpp"
#include "Core/Log/Logger.hpp"
#define MINI_CASE_SENSITIVE

INIParser::INIParser(const fs::path& filePath)
{
	// Create a file instance
	_file = std::make_unique<mINI::INIFile>(filePath.string());
}

mINI::INIStructure& INIParser::GetData()
{
	return _iniData;
}

void INIParser::ReadData()
{
	_file->read(_iniData);
}

bool INIParser::HasKey(StringView section, StringView key)
{
	return _iniData.get(section.data()).has(key.data());
}

const String& INIParser::GetValue(StringView section, StringView key)
{
	static String empty{};
	if (!HasKey(section, key))
		return empty;

	return _iniData[section.data()][key.data()];
}

void INIParser::Update(StringView section, StringView key, StringView value)
{
	_iniData[section.data()][key.data()] = value.data();
}

void INIParser::Set(StringView section, const Vector<std::pair<String, String>>& args)
{
	_iniData[section.data()].set(args);
}

bool INIParser::RemoveKey(StringView section, StringView key)
{
	return _iniData[section.data()].remove(key.data());
}

bool INIParser::RemoveSection(StringView section)
{
	return _iniData.remove(section.data());
}

void INIParser::ClearSection(StringView section)
{
	_iniData[section.data()].clear();
}

void INIParser::Clear()
{
	_iniData.clear();
}

bool INIParser::HasSection(StringView section)
{
	return _iniData.has(section.data());
}

bool INIParser::Write(bool prettyprint)
{
	return _file->write(_iniData, prettyprint);
}

void INIParser::Generate(bool prettyprint)
{
	_file->generate(_iniData, prettyprint);
}
