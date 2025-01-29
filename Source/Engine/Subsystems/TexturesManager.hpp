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
class TexturesManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	TexturesManager(const TexturesManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const TexturesManager&) = delete;

	/** @brief Retrieves the singleton instance of TextureManager. */
	static TexturesManager& Get()
	{
		static TexturesManager textManager;
		return textManager;
	}

	/** @brief Initializes the TextureManager by creating default textures and loading additional textures and icons */
	void Initialize();
	/** @brief Cleans up loaded textures and icons by deleting their OpenGL references. */
	void CleanUp();
	
	/**
	 * @brief Search for a texture in the collection of loaded textures.
	 *
	 * @param relativePath Relative path of the texture to search.
	 * Example: "Character/Ch44_1001_Diffuse.png".
	 * @return Pointer to the texture if found, otherwise nullptr.
	 *
	 * @note The relative path must refer to the 'Assets/Textures/' directory
	 * and must include any subdirectories.
	 */
	const Texture2D* FindTexture(const fs::path& relativePath) const;
	
	/**
	 * @brief Inserts a new texture into the collection, if not already present.
	 *
	 * @param relativePath Relative path of the texture to be loaded.
	 * Example: "Character/Ch44_1001_Diffuse.png".
	 * @param gamma Specifies whether to apply gamma correction.
	 * @return Pointer to the loaded or existing texture.
	 *
	 * @notes The relative path must refer to the 'Assets/Textures/' directory
	 * and must include any subdirectories.
	 */
	const Texture2D* InsertTexture(const fs::path& relativePath, bool gamma = false);
	
	/**
	 * @brief Search for an icon in the loaded icon collection.
	 *
	 * @param relativePath File name of the icon to be searched.
	 * Example: "back-arrow.png".
	 * @return Pointer to the icon if found, otherwise nullptr.
	 *
	 * @notes The relative path must refer to the 'Assets/Icons/' directory 
	 * and must include any subdirectories.
	 */
	const Texture2D* FindTextureIcon(const fs::path& relativePath) const;

	/**
	 * @brief Inserts a new icon into the collection, if not already present.
	 *
	 * @param relativePath File name of the icon to be loaded.
	 * Example: "back-arrow.png".
	 * @return Pointer to the loaded or existing icon.
	 *
	 * @notes The relative path must refer to the 'Assets/Icons/' directory
	 * and must include any subdirectories.
	 */
	const Texture2D* InsertTextureIcon(const fs::path& relativePath);
	
	const Texture2D& GetDefaultDiffuse() const { return _textures.at(0); }
	const Texture2D& GetDefaultSpecular() const { return _textures.at(1); }
	const Texture2D& GetDefaultNormal() const { return _textures.at(2); }
	const auto& GetTextureVector() const { return _textures; }
	const auto& GetTextureIconVector() const { return _icons; }

private:
	TexturesManager() = default;
	~TexturesManager() = default;

	Vector<Texture2D> _textures;
	Vector<Texture2D> _icons;

	// Map storing texture's relative path and its index
	UnorderedMap<fs::path, u32> _textureMap;
	UnorderedMap<fs::path, u32> _iconMap;

	void LoadAllTextures();
	void LoadAllTextureIcons();
};
