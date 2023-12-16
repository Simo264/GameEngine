#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"

class TexturesManager
{
public:
	TexturesManager() = delete;
	~TexturesManager() = delete;

	static void Init();
	
	static Texture2D* LoadTexture(std::filesystem::path textureFilePath);
	static Texture2D* GetTextureByName(const char* filename);

private:
	static std::filesystem::path _texturesDir;

	static array<Texture2D, 10>	_textureBuffer;
	static uint32_t	_numTextures;
};
