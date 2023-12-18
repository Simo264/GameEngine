#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"

// Load and retrieve texture files located within the Textures directory
// ------------------------------------------------------------------------
class TexturesManager
{
public:
	TexturesManager() = delete;
	~TexturesManager() = delete;

	static void Init();
	
	static Texture2D* LoadTexture(std::filesystem::path filePath);
	static Texture2D* GetTexture(std::filesystem::path	filePath);

private:
	static std::filesystem::path _texturesDir;

	static array<Texture2D, 10>	_textureBuffer;
	static uint32_t	_numTextures;
};
