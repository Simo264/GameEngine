#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

/**
 * @namespace Utils
 * 
 * @brief Collection of utility functions
 */
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

	/**
	 * @brief Opens a native file dialog to select files.
	 *
	 * This function displays an open file dialog, allowing the user to select one or multiple files
	 * based on the specified filters and description.
	 *
	 * @param numFilters Number of filter patterns.
	 * @param filter Array of filter patterns (e.g., {"*.txt", "*.png"}).
	 * @param filterDescription Description for the filter (e.g., "Text Files").
	 * @param multipleSelects Boolean flag indicating if multiple files can be selected.
	 * @return A path containing the path of the selected file(s). If multiple files are selected,
	 *         the paths are separated by '|'. Returns an empty String if the dialog is canceled.
	 */
	fs::path OpenFileDialog(i32 numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);

	/**
	 * @brief Opens a native file dialog to save a file.
	 *
	 * This function displays a save file dialog, allowing the user to specify the location
	 * and name of the file to save, based on the provided filters and description.
	 *
	 * @param numFilters Number of filter patterns.
	 * @param filter Array of filter patterns (e.g., {"*.txt", "*.png"}).
	 * @param filterDescription Description for the filter (e.g., "Text Files").
	 * @return A path containing the path of the file to be saved. Returns an empty String if the dialog is canceled.
	 */
	fs::path SaveFileDialog(i32 numFilters, const char* filter[], const char* filterDescription);

	u32 CountFilesInDirectory(const fs::path& directoryPath, bool recursive = false);
};
