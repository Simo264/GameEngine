#pragma once

#include "../Core.hh"
#include "../Texture2D.hh"

#define TEXTURES_MANAGER_MAX_SIZE 100

// Static class TexturesManager:
// load and retrieve texture files located within the Textures/ directory
// ---------------------------------------------------------------------
class TexturesManager
{
public:
	TexturesManager() = delete;
	~TexturesManager() = delete;

	static void Initialize();
	
	// Load texture object by filename in Textures/
	static Texture2D* LoadTexture(Path filePath);
	
	// Get texture object by filename in Textures/
	static Texture2D* GetTexture(Path filePath);

	static void GetTextures(Vector<Texture2D*>& out);

	static void ShutDown();

private:
	inline static Path _texturesDir = std::filesystem::current_path().parent_path() / "Textures";

	inline static UniquePointer<Texture2D[]> _textureBuffer = 
		std::make_unique<Texture2D[]>(TEXTURES_MANAGER_MAX_SIZE);
	
	inline static uint32_t _nTextures = 0;
};
