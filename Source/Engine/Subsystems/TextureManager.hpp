#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Texture2D.hpp"

/**
 * Load and retrieve texture objects from the array
 */
class TextureManager
{
public:
	/**
	 * Load all textures from directory <dirpath>
	 */
	void LoadTexturesFromDir(const fspath& dirpath);

	/**
	 * Destroy all texture objects 
	 */
	void CleanUp();

	/**
	 * Load texture object in texture array
	 */
	Texture2D* LoadTexture(const fspath& filePath, bool gammaCorrection = false);
		
	/**
	 * Load texture object in icon array
	 */
	Texture2D* LoadTextureIcon(const fspath& filePath);

	/**
	 * Retrieve texture object from texture array
	 */
	Texture2D* GetTextureByPath(const fspath& filePath);
	
	/**
	 * Retrieve texture object from icon array
	 */
	Texture2D* GetIconByPath(const fspath& filePath);

	vector<Texture2D> textures;
	vector<Texture2D> icons;
};
