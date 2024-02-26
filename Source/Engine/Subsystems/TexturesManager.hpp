#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/ResourceManager.hpp"

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
	class Texture2D* LoadTexture(Path filePath, bool gammaCorrection = true);
		
	/* Retrieve from memory texture object */
	class Texture2D* GetTextureByPath(Path filePath) const;

	/* Return pointer to first texture object in buffer */
	class Texture2D* Begin() const;
	
	/* Return pointer to last texture object in buffer */
	class Texture2D* End() const;

private:
	class Texture2D* _textureBuffer;
	uint32_t _bufferSize;
};
