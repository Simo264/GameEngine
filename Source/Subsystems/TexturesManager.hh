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
	/* Initialize texture buffer */
	void Initialize() override;
	
	/* Destroy all texture objects and free buffer */
	void ShutDown() override;

	/* Load in memory the texture object by filename */
	Texture2D* LoadTexture(Path filePath, bool gammaCorrection = true);
		
	/* Retrieve from memory texture object */
	Texture2D* GetTextureByPath(Path filePath);

	/* Copy the address of Texture objects into the out vector */
	void CopyTextures(Vector<Texture2D*>& out);

private:
	UniquePointer<Texture2D[]> _textureBuffer;
	uint32_t _bufferSize;
};
