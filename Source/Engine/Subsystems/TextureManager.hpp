#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"
#include "Engine/Graphics/Texture2D.hpp"

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
	 * Load all textures in "Textures/" directory
	 */
	void LoadTextures();

	/**
	 * Load all icons in "Icons/" directory
	 */
	void LoadIcons();

	/**
	 * Load texture object in pool 
	 */
	Texture2D* LoadTexture(const Path& filePath, bool gammaCorrection = true);
		
	/**
	 * Retrieve texture object from pool
	 */
	Texture2D* GetTextureByPath(const Path& filePath);

	/**
	 * Return the address of first element
	 */
	Texture2D* Begin() { return &_pool[0]; }

	/**
	 * Return the address of last element
	 */
	Texture2D* End() { return &_pool[_nTextures]; }

private:
	UniquePointer<Texture2D[]> _pool;
	
	uint32_t _size;
	uint32_t _nTextures;
};
