#include "Utils.hpp"

#include "Core/Log/Logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Utils
{
	vec2i32 StringToVec2i32(StringView str, char delimiter)
	{
		vec2i32 vec{};

		int offset;
		int sepIdx;

		offset = 0;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strX = str.substr(offset, (sepIdx - offset));
		
		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strY = str.substr(offset, (sepIdx - offset));
		
		vec.x = std::atoi(strX.data());
		vec.y = std::atoi(strY.data());

		return vec;
	}
	
	vec2f StringToVec2f(StringView str, char delimiter)
	{
		vec2f vec{};

		int offset;
		int sepIdx;

		offset = 0;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strX = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strY = str.substr(offset, (sepIdx - offset));

		vec.x = std::atof(strX.data());
		vec.y = std::atof(strY.data());

		return vec;
	}

	vec3i32 StringToVec3i32(StringView str, char delimiter)
	{
		vec3i32 vec{};

		int offset;
		int sepIdx;

		offset = 0;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strX = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strY = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strZ = str.substr(offset, (sepIdx - offset));

		vec.x = std::atoi(strX.data());
		vec.y = std::atoi(strY.data());
		vec.z = std::atoi(strZ.data());

		return vec;
	}

	vec3f StringToVec3f(StringView str, char delimiter)
	{
		vec3f vec{};

		int offset;
		int sepIdx;

		offset = 0;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strX = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strY = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strZ = str.substr(offset, (sepIdx - offset));

		vec.x = std::atof(strX.data());
		vec.y = std::atof(strY.data());
		vec.z = std::atof(strZ.data());

		return vec;
	}

	bool StringToBool(StringView str)
	{
		return (str == "1" || str == "true");
	}

	u8* LoadImageData(StringView strPath, i32& width, i32& height, i32& nChannels)
	{
		u8* data = stbi_load(strPath.data(), &width, &height, &nChannels, 0);
		return data;
	}

	void FreeImageData(u8* data)
	{
		stbi_image_free(reinterpret_cast<void*>(data));
	}
};