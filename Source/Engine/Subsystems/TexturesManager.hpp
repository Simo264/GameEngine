#pragma once

#include "Core.hpp"
#include "Engine/Subsystems/ResourceManager.hpp"
#include "Engine/Graphics/Texture2D.hpp"

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
	Texture2D* GetTextureByPath(Path filePath) const;

	Texture2D* Begin() const { return &_textureBuffer[0]; }
	Texture2D* End() const { return &_textureBuffer[0] + _bufferSize; }

private:
	UniquePointer<Texture2D[]> _textureBuffer;
	uint32_t _bufferSize;
};
