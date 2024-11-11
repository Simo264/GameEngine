#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

namespace Utils
{
	i32 StringToI32(StringView str);

	f32 StringToF32(StringView str);

	/* From string "50,50" -> to Vec2i(50,50) */
	vec2i StringToVec2i(StringView str, char delimiter = ',');

	/* From string "50,50" -> to Vec2f(50,50) */
	vec2f StringToVec2f(StringView str, char delimiter = ',');

	/* From string "50,50,50" -> to Vec3i32(50,50,50) */
	vec3i StringToVec3i(StringView str, char delimiter = ',');
	
	/* From string "50,50,50" -> to Vec3f(50,50,50) */
	vec3f StringToVec3f(StringView str, char delimiter = ',');

	/* Parse strings { "1" "0" "true" "false" } to boolean */
	bool StringToBool(StringView str);

	/* Load image data with stbi_load function */
	u8* LoadImageData(const fs::path& imagePath, i32& width, i32& height, i32& nChannels);

	/* Call stbi_image_free function */
	void FreeImageData(u8* data);
};
