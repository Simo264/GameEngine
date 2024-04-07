#include "INIFileParser.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

INIFileParser::INIFileParser(const Path& filepath)
{
	/* Create a file instance */
	_file = std::make_unique<mINI::INIFile>(filepath.string());
}

Vec2i32 INIFileParser::StringToVec2i(String& str, const char* delimiter)
{
	Vec2i32 vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stoi(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.y = std::stoi(token);
	return vec;
}

Vec2f INIFileParser::StringToVec2f(String& str, const char* delimiter)
{
	Vec2f vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stof(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.y = std::stof(token);
	return vec;
}

Vec3i32 INIFileParser::StringToVec3i(String& str, const char* delimiter)
{
	Vec3i32 vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stoi(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.y = std::stoi(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.z = std::stoi(token);
	return vec;
}

Vec3f INIFileParser::StringToVec3f(String& str, const char* delimiter)
{
	Vec3f vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stof(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.y = std::stof(token);
	token = strtok_s(NULL, delimiter, &next_token);
	vec.z = std::stof(token);
	return vec;
}

bool INIFileParser::StringToBool(const String& str)
{
	return (str.compare("1") == 0 || str.compare("true") == 0);
}	

void INIFileParser::PrintContent()
{
	for (auto const& it : _iniData)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		CONSOLE_TRACE("[{}]", section);
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			CONSOLE_TRACE("{}={}", key, value);
		}
	}
}
