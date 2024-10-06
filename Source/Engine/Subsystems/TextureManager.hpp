#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

/**
 * Load and retrieve texture objects
 */
class TextureManager
{
public:
	TextureManager(const TextureManager&) = delete;
	void operator=(const TextureManager&) = delete;

	/**
	 * Return the instance of this TextureManager singleton class
	 */
	static TextureManager& Get()
	{
		static TextureManager textManager;
		return textManager;
	}

	/**
	 * Load all textures and icons from default directories
	 */
	void Initialize();

	/**
	 * Destroy all texture objects 
	 */
	void CleanUp();

	/**
	 * Returns a reference to the mapped value of the element with specified key.
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Texture2D& GetTextureByPath(const fs::path& path);
	
	/**
	 * Returns a reference to the mapped value of the element with specified key.
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Texture2D& GetIconByPath(const fs::path& path);
	
	Texture2D& GetDefaultDiffuse() { return _textures.at("#default_diffuse"); }
	Texture2D& GetDefaultSpecular() { return _textures.at("#default_specular"); }
	Texture2D& GetDefaultNormal() { return _textures.at("#default_normal"); }
	Texture2D& GetDefaultHeight() { return _textures.at("#default_height"); }
	UnorderedMap<String, Texture2D>& GetTextures() { return _textures; }

private:
	TextureManager() = default;
	~TextureManager() = default;

	/**
	 * First: the texture file path in string format
	 * Second: the texture object
	 */
	UnorderedMap<String, Texture2D> _textures;

	/**
	 * First: the texture file path in string format
	 * Second: the texture object
	 */
	UnorderedMap<String, Texture2D> _icons;

	void LoadTextures();
	void LoadIcons();
};
