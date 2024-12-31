#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

/**
 * @class TextureManager.
 * 
 * @brief Manages the loading, storage, and retrieval of textures and icons.
 * 
 * This class implements the singleton pattern to ensure only one instance of TextureManager
 * exists throughout the application.
 */
class TextureManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	TextureManager(const TextureManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const TextureManager&) = delete;

	/** @brief Retrieves the singleton instance of TextureManager. */
	static TextureManager& Get()
	{
		static TextureManager textManager;
		return textManager;
	}

	/** @brief Initializes the TextureManager by creating default textures and loading additional textures and icons */
	void Initialize();
	/** @brief Cleans up loaded textures and icons by deleting their OpenGL references. */
	void CleanUp();
	
	/**
	 * @brief Retrieves a texture by its file path.
	 * 
	 * @param path Path to the texture file.
	 * @return Reference to the corresponding Texture2D object.
	 * 
	 * @throw std::runtime_error if the texture does not exist.
	 */
	Texture2D& GetTextureByPath(const fs::path& path);
	
	/**
	 * @brief Retrieves a icon by its file path.
	 *
	 * @param path Path to the icon file.
	 * @return Reference to the corresponding Texture2D object.
	 *
	 * @throw std::runtime_error if the icon does not exist.
	 */
	Texture2D& GetIconByPath(const fs::path& path);
	
	/** @brief Retrieves the default diffuse texture. */
	Texture2D& GetDefaultDiffuse() { return _textures.at("#default_diffuse"); }
	/** @brief Retrieves the default specular  texture. */
	Texture2D& GetDefaultSpecular() { return _textures.at("#default_specular"); }
	/** @brief Retrieves the default normal  texture. */
	Texture2D& GetDefaultNormal() { return _textures.at("#default_normal"); }
	/** @brief Retrieves all loaded textures. */
	const auto& GetTextures() const { return _textures; }

private:
	TextureManager() = default;
	~TextureManager() = default;

	/**
	 * @brief Stores textures by their file path.
	 */
	UnorderedMap<fs::path, Texture2D> _textures;

	/**
	 * @brief Stores icons by their file path.
	 */
	UnorderedMap<fs::path, Texture2D> _icons;

	/** @brief Loads all textures from disk. */
	void LoadTextures();
	/** @brief Loads all icons from disk. */
	void LoadIcons();
};
