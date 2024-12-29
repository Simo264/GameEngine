#include "Utils.hpp"

#include "Core/Log/Logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Utils
{
	i32 StringToI32(StringView str)
	{
		return std::stoi(str.data());
	}

	f32 StringToF32(StringView str)
	{
		return std::stof(str.data());
	}

	vec2i StringToVec2i(StringView str, char delimiter)
	{
		vec2i vec{};

		u32 offset = 0;
		u32 sepIdx = str.find_first_of(delimiter, offset);
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

		u32 offset = 0;
		u32 sepIdx = str.find_first_of(delimiter, offset);
		StringView strX = str.substr(offset, (sepIdx - offset));

		offset = sepIdx + 1;
		sepIdx = str.find_first_of(delimiter, offset);
		StringView strY = str.substr(offset, (sepIdx - offset));

		vec.x = std::atof(strX.data());
		vec.y = std::atof(strY.data());

		return vec;
	}

	vec3i StringToVec3i(StringView str, char delimiter)
	{
		vec3i vec{};

		u32 offset = 0;
		u32 sepIdx = str.find_first_of(delimiter, offset);
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

		u32 offset = 0;
		u32 sepIdx = str.find_first_of(delimiter, offset);
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

	u8* LoadImageData(const fs::path& imagePath, i32& width, i32& height, i32& nChannels)
	{
		u8* data = stbi_load(imagePath.string().c_str(), &width, &height, &nChannels, 0);
		return data;
	}

	void FreeImageData(u8* data)
	{
		stbi_image_free(reinterpret_cast<void*>(data));
	}
};