#include "ImageLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ImageLoader
{
	u8* LoadImageData(const fs::path& absolutePath, i32& width, i32& height, i32& nChannels)
	{
		u8* data = stbi_load(absolutePath.string().c_str(), &width, &height, &nChannels, 0);
		return data;
	}

	void FreeImageData(u8* data)
	{
		stbi_image_free(reinterpret_cast<void*>(data));
	}
}
