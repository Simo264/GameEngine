#include "INIFileParser.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

INIFileParser::INIFileParser(const fspath& filepath)
{
	/* Create a file instance */
	_file = std::make_unique<mINI::INIFile>(filepath.string());
}

vec2i32 INIFileParser::StringToVec2i32(string& str, const char* delimiter)
{
	vec2i32 vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stoi(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.y = std::stoi(token);
	return vec;
}

vec2f INIFileParser::StringToVec2f(string& str, const char* delimiter)
{
	vec2f vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stof(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.y = std::stof(token);
	return vec;
}

vec3i32 INIFileParser::StringToVec3i32(string& str, const char* delimiter)
{
	vec3i32 vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stoi(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.y = std::stoi(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.z = std::stoi(token);
	return vec;
}

vec3f INIFileParser::StringToVec3f(string& str, const char* delimiter)
{
	vec3f vec;
	char* token = nullptr;
	char* next_token = nullptr;
	token = strtok_s(str.data(), delimiter, &next_token);
	vec.x = std::stof(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.y = std::stof(token);
	token = strtok_s(nullptr, delimiter, &next_token);
	vec.z = std::stof(token);
	return vec;
}

bool INIFileParser::StringToBool(const string& str)
{
	return (str.compare("1") == 0 || str.compare("true") == 0);
}	
