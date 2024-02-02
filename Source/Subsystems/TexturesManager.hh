#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"

#define TEXTURES_MANAGER_MAX_SIZE 100

/* ------------------------------------------------
	Static class TexturesManager:
	load in and retrieve from memory texture objects
	------------------------------------------------- */
class TexturesManager
{
public:
	TexturesManager() = delete;
	~TexturesManager() = delete;

	static void Initialize();
	
	/* Load in memory the texture object by filename in Textures directory */
	static Texture2D* LoadTexture(Path filePath, bool gammaCorrection = true);
	
	/* Retrieve from memory texture object by filename in Textures */
	static Texture2D* GetTexture(Path filePath);

	static void GetTextures(Vector<Texture2D*>& out);

	static void ShutDown();

private:
	inline static Path _texturesDir = ROOT_PATH / "Textures";

	inline static UniquePointer<Texture2D[]> _textureBuffer = 
		std::make_unique<Texture2D[]>(TEXTURES_MANAGER_MAX_SIZE);
	
	inline static uint32_t _nTextures = 0;
};
