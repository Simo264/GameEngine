#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

/**
 * @namespace Utils
 * 
 * @brief Collection of utility functions for string conversion and image loading
 */
namespace Utils
{
	/**
	 * @brief Converts a string to a 32-bit integer.
	 * 
	 * @param str String view containing the number to convert.
	 * 
	 * @return Converted 32-bit integer.
	 */
	i32 StringToI32(StringView str);

	/**
	 * @brief Converts a string to a 32-bit floating-point number.
	 * 
	 * @param str String view containing the number to convert.
	 * 
	 * @return Converted 32-bit floating-point number.
	 */
	f32 StringToF32(StringView str);

	/**
	 * @brief Converts a delimited string to a 2D integer vector.
	 * 
	 * @param str String view containing the vector components.
	 * @param delimiter Character separating the components.
	 * 
	 * @return 2D integer vector.
	 */
	vec2i StringToVec2i(StringView str, char delimiter = ',');

	/**
	 * @brief Converts a delimited string to a 2D floating-point vector.
	 *
	 * @param str String view containing the vector components.
	 * @param delimiter Character separating the components.
	 *
	 * @return 2D floating-point vector.
	 */
	vec2f StringToVec2f(StringView str, char delimiter = ',');

	/**
	 * @brief Converts a delimited string to a 3D integer vector.
	 *
	 * @param str String view containing the vector components.
	 * @param delimiter Character separating the components.
	 *
	 * @return 3D integer vector.
	 */
	vec3i StringToVec3i(StringView str, char delimiter = ',');
	
	/**
	 * @brief Converts a delimited string to a 3D floating-point vector.
	 *
	 * @param str String view containing the vector components.
	 * @param delimiter Character separating the components.
	 *
	 * @return 3D floating-point vector.
	 */
	vec3f StringToVec3f(StringView str, char delimiter = ',');

	/**
	 * @brief Converts a string to a boolean value.
	 * 
	 * @param str String view containing the boolean representation.
	 * 
	 * @return True if the string is "1" or "true", otherwise false.
	 */
	bool StringToBool(StringView str);

	/**
	 * @brief Loads image data from a file.
	 * 
	 * @param imagePath Path to the image file.
	 * @param width Reference to store the width of the image.
	 * @param height Reference to store the height of the image.
	 * @param nChannels Reference to store the number of channels.
	 * 
	 * @return Pointer to the loaded image data.
	 */
	u8* LoadImageData(const fs::path& imagePath, i32& width, i32& height, i32& nChannels);

	/**
	 * @brief Frees allocated image data.
	 * 
	 * @param data Pointer to the image data to free.
	 */
	void FreeImageData(u8* data);
};
