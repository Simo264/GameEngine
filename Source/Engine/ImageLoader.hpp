#pragma once

#include "Core/Core.hpp"

namespace ImageLoader
{
	/**
	 * @brief Loads image data from a file.
	 *
	 * @param absolutePath Path to the image file.
	 * @param width Reference to store the width of the image.
	 * @param height Reference to store the height of the image.
	 * @param nChannels Reference to store the number of channels.
	 *
	 * @return Pointer to the loaded image data.
	 */
	u8* LoadImageData(const fs::path& absolutePath, i32& width, i32& height, i32& nChannels);

	/**
	 * @brief Frees allocated image data.
	 *
	 * @param data Pointer to the image data to free.
	 */
	void FreeImageData(u8* data);
}
