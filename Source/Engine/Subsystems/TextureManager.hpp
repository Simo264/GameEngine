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
	 * @return
	 * The instance of this TextureManager singleton class
	 */
	static TextureManager& Get()
	{
		static TextureManager textManager;
		return textManager;
	}

	/**
	 * @brief
	 * Load all textures and icons from default directories
	 */
	void Initialize();

	/**
	 * @brief
	 * Destroy all texture objects 
	 */
	void CleanUp();

	/**
	 * @return
	 * A reference to the mapped value of the element with specified key.
	 * 
	 * @throw
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Texture2D& GetTextureByPath(const fs::path& path);
	
	/**
	 * @return
	 * A reference to the mapped value of the element with specified key.
	 * 
	 * @throw
	 * If no such element exists, an exception of type std::out_of_range is thrown.
	 */
	Texture2D& GetIconByPath(const fs::path& path);
	
	Texture2D& GetDefaultDiffuse() { return _textures.at("#default_diffuse"); }
	Texture2D& GetDefaultSpecular() { return _textures.at("#default_specular"); }
	Texture2D& GetDefaultNormal() { return _textures.at("#default_normal"); }
	auto& GetTextures() { return _textures; }

private:
	TextureManager() = default;
	~TextureManager() = default;

	/**
	 * First: the texture file path
	 * Second: the texture object
	 */
	UnorderedMap<fs::path, Texture2D> _textures;

	/**
	 * First: the texture file path
	 * Second: the texture object
	 */
	UnorderedMap<fs::path, Texture2D> _icons;

	void LoadTextures();
	void LoadIcons();
};
