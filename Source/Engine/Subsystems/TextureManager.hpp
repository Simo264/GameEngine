#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

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
	void LoadTexture(const Texture2D& texture) { _textures.push_back(texture); }

		
	/**
	 * Load texture object in icon array
	 */
	Texture2D& LoadTextureIcon(int target, const fs::path& filePath);
	void LoadTextureIcon(const Texture2D& texture) { _icons.push_back(texture); }

	/**
	 * Retrieve texture object from texture array
	 */
	Texture2D* GetTextureByPath(const fs::path& filePath);
	Texture2D* GetTextureAt(int i) { return &_textures.at(i); }
	
	/**
	 * Retrieve texture object from icon array
	 */
	Texture2D* GetIconByPath(const fs::path& filePath);
	Texture2D* GetIconAt(int i) { return &_icons.at(i); }

private:
	vector<Texture2D> _textures;
	vector<Texture2D> _icons;
};
