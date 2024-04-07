#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"

/**
 * Load and retrieve texture objects from memory 
 */
class TextureManager : public Manager<TextureManager>
{
public:
	/**
	 * Initialize texture pool
	 */
	void Initialize() override;
	
	/**
	 * Destroy all texture objects and free pool
	 */
	void CleanUp() override;

	/**
	 * Load texture object in pool 
	 */
	class Texture2D& LoadTexture(const Path& filePath, bool gammaCorrection = true);
		
	/**
	 * Retrieve texture object from pool
	 */
	class Texture2D* GetTextureByPath(const Path& filePath) const;

	/**
	 * Return begin iterator
	 */
	constexpr auto Begin() { return _textures.begin(); }

	/**
	 * Return end iterator
	 */
	constexpr auto End() { return _textures.end(); }

private:
	Vector<class Texture2D*> _textures;
};
