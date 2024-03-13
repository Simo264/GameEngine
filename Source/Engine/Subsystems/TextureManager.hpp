#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"

/* ------------------------------------------------
	TextureManager subsystem:
	load and retrieve from memory texture objects
	------------------------------------------------- */
class TextureManager : public Manager<TextureManager>
{
public:
	/* Initialize texture buffer */
	void Initialize() override;
	
	/* Destroy all texture objects and free buffer */
	void CleanUp() override;

	/* Load in memory the texture object by filename */
	class Texture2D& LoadTexture(const Path& filePath, bool gammaCorrection = true);
		
	/* Retrieve from memory texture object */
	class Texture2D& GetTextureByPath(const Path& filePath) const;

	/* Return begin iterator */
	constexpr auto Begin() { return _textures.begin(); }

	/* Return end iterator */
	constexpr auto End() { return _textures.end(); }

private:
	Vector<class Texture2D*> _textures;
};
