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
	 * @brief Finds an existing texture by its relative path.
	 * This method searches for a texture that has already been loaded using the given relative path.
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 * 
	 * @return Pointer to the found Texture2D, or nullptr if not found.
	 */
	const Texture2D* FindTexture(const fs::path& relative) const;
	
	/**
	 * @brief Creates and loads a new texture from the given relative path.
	 * If the texture does not exist, it is loaded and stored for future use.
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 * 
	 * @return Pointer to the newly created Texture2D.
	 */
	const Texture2D* CreateTexture(const fs::path& relative);
	
	/**
	 * @brief Retrieves an existing texture or creates a new one if not found.
	 * If the texture is already loaded, it returns the existing instance; otherwise, it creates a new one.
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 * 
	 * @return Pointer to the existing or newly created Texture2D.
	 */
	const Texture2D* GetOrCreateTexture(const fs::path& relative);

	/**
	 * @brief Finds an existing icon by its relative path.
	 * This method searches for an icon that has already been loaded using the given relative path.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 * 
	 * @return Pointer to the found Texture2D, or nullptr if not found.
	 */
	const Texture2D* FindIcon(const fs::path& relative) const;

	/**
	 * @brief Creates and loads a new icon from the given relative path.
	 * If the icon does not exist, it is loaded and stored for future use.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 * 
	 * @return Pointer to the newly created Texture2D.
	 */
	const Texture2D* CreateIcon(const fs::path& relative);

	/**
	 * @brief Retrieves an existing icon or creates a new one if not found.
	 * If the icon is already loaded, it returns the existing instance; otherwise, it creates a new one.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 * 
	 * @return Pointer to the existing or newly created Texture2D.
	 */
	const Texture2D* GetOrCreateIcon(const fs::path& relative);

	const fs::path* GetTexturePath(u32 textureID) const;
	
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

	// Map storing the texture's index and texture's path
	Vector<std::pair<u32, fs::path>> _texturePaths;
	Vector<std::pair<u32, fs::path>> _iconPaths;
};
