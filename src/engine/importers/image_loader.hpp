#pragma once

#include "Core/Types.hpp"
#include "Core/FileSystem.hpp"

class ImageLoader
{
public:
	ImageLoader() : 
		width{},
		height{},
		nrChannels{},
		internalFormat{}
	{}
	~ImageLoader() = default;

	byte* LoadImageData(const fs::path& absolutePathToFile);
	
	/** @brief Frees the memory allocated for image data. */
	void FreeImageData(void* data);

	/** @brief Sets the vertical flip state for image loading. */
	void SetFlipVerticallyOnLoad(bool b);

	i32 width;
	i32 height;
	i32 nrChannels;
	u32 internalFormat;
};