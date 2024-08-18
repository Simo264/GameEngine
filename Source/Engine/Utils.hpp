#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

namespace Utils
{
	/* From string "50,50" -> to Vec2i(50,50) */
	vec2i32 StringToVec2i32(String& str, const char* delimiter = ",");

	/* From string "50,50" -> to Vec2f(50,50) */
	vec2f StringToVec2f(String& str, const char* delimiter = ",");

	/* From string "50,50,50" -> to Vec3i32(50,50,50) */
	vec3i32 StringToVec3i32(String& str, const char* delimiter = ",");
	
	/* From string "50,50,50" -> to Vec3f(50,50,50) */
	vec3f StringToVec3f(String& str, const char* delimiter = ",");

	/* Parse strings { "1" "0" "true" "false" } to boolean */
	bool StringToBool(const String& str);

	/* Load image data with stbi_load function */
	Byte* LoadImageData(const fs::path& path, i32& width, i32& height, i32& nChannels);

	/* Call stbi_image_free function */
	void FreeImageData(Byte* data);
};
