#pragma once

#include "ResourceManager.hh"
#include "../Texture2D.hh"

/* ------------------------------------------------
	Resource manager class TexturesManager:
	load in and retrieve from memory texture objects
	------------------------------------------------- */

class TexturesManager : public ResourceManagerInterface<TexturesManager>
{
public:
	void Initialize() override;
	void ShutDown() override;

	/* Load in memory the texture object by filename */
	Texture2D* LoadTexture(Path filePath, bool gammaCorrection = true);
		
	/* Retrieve from memory texture object by filename */
	Texture2D* GetTexture(Path filePath);

	/* Copy the address of Texture objects into out vector */
	void GetTextures(Vector<Texture2D*>& out);

private:
	UniquePointer<Texture2D[]> _textureBuffer;
	uint32_t _bufferSize;
};
