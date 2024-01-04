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

	static void Initialize();
	
	// Load texture object by filename in Textures/...
	static Texture2D* LoadTexture(Path filePath);
	
	// Get texture object by filename in Textures/...
	static Texture2D* GetTexture(Path filePath);

private:
	inline static Path _texturesDir = std::filesystem::current_path().parent_path() / "Textures";

	inline static Array<Texture2D, 10>	_textureBuffer = Array<Texture2D, 10>();
	inline static uint32_t	_numTextures = 0;
};
