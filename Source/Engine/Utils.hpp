#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

namespace Utils
{
	/** @brief Converts a string to a 32-bit integer. */
	i32 StringToI32(StringView str);

	/** @brief Converts a string to a 32-bit floating-point number. */
	f32 StringToF32(StringView str);

	/** @brief Converts a delimited string to a 2D integer vector. */
	vec2i StringToVec2i(StringView str, char delimiter = ',');

	/** @brief Converts a delimited string to a 2D floating-point vector. */
	vec2f StringToVec2f(StringView str, char delimiter = ',');

	/** @brief Converts a delimited string to a 3D integer vector. */
	vec3i StringToVec3i(StringView str, char delimiter = ',');
	
	/** @brief Converts a delimited string to a 3D floating-point vector. */
	vec3f StringToVec3f(StringView str, char delimiter = ',');

	/** @brief Converts a string to a boolean value. */
	bool StringToBool(StringView str);

	/**
	 * @brief Counts the number of files in a directory.
	 *
	 * This function counts the number of files in the specified directory. If the recursive flag is set to true,
	 * it will count the files in all subdirectories as well.
	 */
	u32 CountFilesInDirectory(const fs::path& directoryPath, bool recursive = false);
};
