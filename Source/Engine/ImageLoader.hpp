#pragma once

#include "Core/Core.hpp"

namespace ImageLoader
{
	/**
	 * @brief Loads an LDR (Low Dynamic Range) image into memory.
	 *
	 * This function reads an image file and returns a pointer to the image data
	 * stored as 8-bit per channel. Suitable for standard image formats like PNG and JPG.
	 *
	 * @param absolutePath The absolute path to the image file.
	 * @param width Reference to store the image width.
	 * @param height Reference to store the image height.
	 * @param nChannels Reference to store the number of channels in the image (e.g., 3 for RGB, 4 for RGBA).
	 * @return A pointer to the loaded image data, or nullptr if loading fails.
	 */
	u8* LoadLDRImage(const fs::path& absolutePath, i32& width, i32& height, i32& nChannels);

	/**
	 * @brief Loads an HDR (High Dynamic Range) image into memory.
	 *
	 * This function reads an HDR image file and returns a pointer to the image data
	 * stored as floating-point values. It is useful for rendering applications that
	 * require high precision in lighting calculations.
	 *
	 * @param absolutePath The absolute path to the image file.
	 * @param width Reference to store the image width.
	 * @param height Reference to store the image height.
	 * @param nChannels Reference to store the number of channels in the image (e.g., 3 for RGB, 4 for RGBA).
	 * @return A pointer to the loaded image data as floating-point values, or nullptr if loading fails.
	 */
	f32* LoadHDRImage(const fs::path& absolutePath, i32& width, i32& height, i32& nChannels);

	/**
	 * @brief Frees the memory allocated for image data.
	 *
	 * This function releases the memory allocated for an image loaded with `LoadLDRImage` or `LoadHDRImage`.
	 * It should be called after processing the image to prevent memory leaks.
	 *
	 * @param data Pointer to the image data to be freed.
	 */
	void FreeImageData(void* data);

}
