#include "INIFileManager.hpp"
#include "Engine/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

INIFileManager::INIFileManager(Path filepath)
{
	/* Create a file instance */
	_file = std::make_unique<mINI::INIFile>(filepath.string());
}

template<>
Vec2i INIFileManager::StringToVec2(const String& str, char sep)
{
	int tmp = str.find(sep);
	int x = std::stoi(str.substr(0, tmp));
	int y = std::stoi(str.substr(tmp + 1));
	return Vec2i(x, y);
}

template<>
Vec2f INIFileManager::StringToVec2(const String& str, char sep)
{
	int tmp = str.find(sep);
	float x = std::stof(str.substr(0, tmp));
	float y = std::stof(str.substr(tmp + 1));
	return Vec2f(x, y);
}

template<>
Vec3i INIFileManager::StringToVec3(const String& str, char sep)
{
	Vec3i vec;
	IStringStream iss(str);
	String seg;
	std::getline(iss, seg, sep);
	vec.x = std::stoi(seg);
	std::getline(iss, seg, sep);
	vec.y = std::stoi(seg);
	std::getline(iss, seg, sep);
	vec.z = std::stoi(seg);
	return vec;
}

template<>
Vec3f INIFileManager::StringToVec3(const String& str, char sep)
{
	Vec3f vec;
	IStringStream iss(str);
	String seg;
	std::getline(iss, seg, sep);
	vec.x = std::stof(seg);
	std::getline(iss, seg, sep);
	vec.y = std::stof(seg);
	std::getline(iss, seg, sep);
	vec.z = std::stof(seg);
	return vec;
}

template<>
String INIFileManager::Vec2ToString(const Vec2i& v, char sep)
{
	OStringStream oss;
	oss << v.x << sep << v.y;
	return oss.str();
}

template<>
String INIFileManager::Vec2ToString(const Vec2f& v, char sep)
{
	OStringStream oss;
	oss << v.x << sep << v.y;
	return oss.str();
}

template<>
String INIFileManager::Vec3ToString(const Vec3i& v, char sep)
{
	OStringStream oss;
	oss << v.x << sep << v.y << sep << v.z;
	return oss.str();
}

template<>
String INIFileManager::Vec3ToString(const Vec3f& v, char sep)
{
	OStringStream oss;
	oss << v.x << sep << v.y << sep << v.z;
	return oss.str();
}

bool INIFileManager::StringToBool(const String& str)
{
	if (str.compare("1") == 0 || str.compare("true") == 0)
		return true;
	
	return false;
}	

String INIFileManager::BoolToString(const bool& b)
{
	if (b)
		return "true";

	return "false";
}

void INIFileManager::PrintContent()
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
