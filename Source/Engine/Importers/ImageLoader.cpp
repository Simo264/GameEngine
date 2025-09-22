#include "ImageLoader.hpp"

#include "Core/Logger.hpp"
#include "Engine/Graphics/Texture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

byte* ImageLoader::LoadImageData(const fs::path& absolutePathToFile)
{
	if (!fs::exists(absolutePathToFile))
	{
		CONSOLE_ERROR("Image file does not exist {}", absolutePathToFile.string());
		return nullptr;
	}

	// When to Apply Gamma Correction to Textures?
	// Gamma correction is only applied to colour diffuse(albedo) textures because the colours
	// must be interpreted in gamma space(sRGB).
	// Other textures, such as normal, specular, metallic, roughness, are linear data and should not
	// undergo gamma correction.
	auto gammaCorrection = false;
	auto filename = absolutePathToFile.filename().string();
	std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
	if (filename.find("diffuse") != String::npos || filename.find("albedo") != String::npos)
		gammaCorrection = true;

	auto data = stbi_load(absolutePathToFile.string().c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		CONSOLE_ERROR("Failed to load image {}", absolutePathToFile.string());
		return nullptr;
	}

	// From https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
	// GL_RGB:         gamma correction: no;   alpha component: no
	// GL_RGBA:        gamma correction: no;   alpha component: yes
	// GL_SRGB:        gamma correction: yes;  alpha component: no
	// GL_SRGB_ALPHA:  gamma correction: yes;  alpha component: yes
	internalFormat = static_cast<u32>(Texture2DInternalFormat::RGB8);
	switch (nrChannels)
	{
		case 1:
			internalFormat = static_cast<u32>(Texture2DInternalFormat::R8);
			break;
		case 2:
			internalFormat = static_cast<u32>(Texture2DInternalFormat::RG8);
			break;
		case 3:
			internalFormat = static_cast<u32>(gammaCorrection ? Texture2DInternalFormat::SRGB8 : Texture2DInternalFormat::RGB8);
			break;
		case 4:
			internalFormat = static_cast<u32>(gammaCorrection ? Texture2DInternalFormat::SRGB8_ALPHA8 : Texture2DInternalFormat::RGBA8);
			break;
	}
	return reinterpret_cast<byte*>(data);
}

void ImageLoader::FreeImageData(void* data)
{
	stbi_image_free(data);
}

void ImageLoader::SetFlipVerticallyOnLoad(bool b)
{
	stbi_set_flip_vertically_on_load(b);
}
