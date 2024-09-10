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

	Texture2D& GetTextureByPath(const fs::path& filePath);
	
	Texture2D& GetIconByPath(const fs::path& filePath);
	
	const Texture2D& GetDefaultDiffuse() const	{ return _textures.at("#default_diffuse"); }
	const Texture2D& GetDefaultSpecular() const { return _textures.at("#default_specular"); }
	const Texture2D& GetDefaultNormal() const { return _textures.at("#default_normal"); }
	const Texture2D& GetDefaultHeight() const { return _textures.at("#default_height"); }
	constexpr const UnorderedMap<String, Texture2D>& GetTextures() { return _textures; }

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
