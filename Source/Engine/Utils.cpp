#include "Utils.hpp"

namespace Utils
{
	vec2i32 StringToVec2i32(string& str, const char* sep)
	{
		vec2i32 vec{};
		char* token = nullptr;
		char* next_token = nullptr;
		token = strtok_s(str.data(), sep, &next_token);
		vec.x = std::stoi(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.y = std::stoi(token);
		return vec;
	}
	
	vec2f StringToVec2f(string& str, const char* sep)
	{
		vec2f vec{};
		char* token = nullptr;
		char* next_token = nullptr;
		token = strtok_s(str.data(), sep, &next_token);
		vec.x = std::stof(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.y = std::stof(token);
		return vec;
	}

	vec3i32 StringToVec3i32(string& str, const char* sep)
	{
		vec3i32 vec{};
		char* token = nullptr;
		char* next_token = nullptr;
		token = strtok_s(str.data(), sep, &next_token);
		vec.x = std::stoi(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.y = std::stoi(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.z = std::stoi(token);
		return vec;
	}

	vec3f StringToVec3f(string& str, const char* sep)
	{
		vec3f vec{};
		char* token = nullptr;
		char* next_token = nullptr;
		token = strtok_s(str.data(), sep, &next_token);
		vec.x = std::stof(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.y = std::stof(token);
		token = strtok_s(nullptr, sep, &next_token);
		vec.z = std::stof(token);
		return vec;
	}

	bool StringToBool(const string& str)
	{
		return (str.compare("1") == 0 || str.compare("true") == 0);
	}
};