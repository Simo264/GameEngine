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
	void LoadTexturesFromDir(const fs::path& dirpath);

	/**
	 * Destroy all texture objects 
	 */
	void CleanUp();

	/**
	 * Load texture object in texture array
	 */
	Texture2D& LoadTexture(int target, const fs::path& filePath, bool gammaCorrection);
		
	/**
	 * Load texture object in icon array
	 */
	Texture2D& LoadTextureIcon(int target, const fs::path& filePath);

	/**
	 * Retrieve texture object from texture array
	 */
	Texture2D* GetTextureByPath(const fs::path& filePath);
	
	/**
	 * Retrieve texture object from icon array
	 */
	Texture2D* GetIconByPath(const fs::path& filePath);

	vector<Texture2D> textures;
	vector<Texture2D> icons;
};
