#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"

/**
 * @class TextureManager.
 * 
 * @brief Manages the loading, storage, and retrieval of textures and icons.
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
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 * 
	 * @return The corresponding Texture2D if found, otherwise an invalid Texture2D. 
	 * If the texture is not found, this function returns an invalid `Texture2D` object.
	 * To verify whether the returned texture is valid, use the `IsValid()` method.
	 */
	Texture2D FindTexture(const fs::path& relative) const;
	
	/**
	 * @brief Creates and loads a new texture from the given relative path.
	 * If the texture does not exist, it is loaded and stored for future use.
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 * 
	 * @return The loaded Texture2D object.
	 */
	Texture2D CreateTexture(const fs::path& relative);
	
	/**
	 * @brief Retrieves an existing texture or creates a new one if not found.
	 * If the texture is already loaded, it returns the existing instance; otherwise, it creates a new one.
	 *
	 * @param relative Relative path to the texture inside "Assets/Textures".
	 * Example: "skybox/front.jpg"
	 */
	Texture2D GetOrCreateTexture(const fs::path& relative);

	/**
	 * @brief Finds an existing icon by its relative path.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 * 
	 * @return The corresponding Texture2D if found, otherwise an invalid Texture2D. 
	 * If the texture is not found, this function returns an invalid `Texture2D` object.
	 * To verify whether the returned texture is valid, use the `IsValid()` method.
	 */
	Texture2D FindIcon(const fs::path& relative) const;

	/**
	 * @brief Creates and loads a new icon from the given relative path.
	 * If the icon does not exist, it is loaded and stored for future use.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 * 
	 * @return The loaded Texture2D object.
	 */
	Texture2D CreateIcon(const fs::path& relative);

	/**
	 * @brief Retrieves an existing icon or creates a new one if not found.
	 * If the icon is already loaded, it returns the existing instance; otherwise, it creates a new one.
	 *
	 * @param relative Relative path to the icon inside "Assets/Icons".
	 * Example: "back-arrow.png"
	 */
	Texture2D GetOrCreateIcon(const fs::path& relative);

	Texture2D GetDefaultDiffuse() const { return _defaultDiffuse; }
	Texture2D GetDefaultSpecular() const { return _defaultSpecular; }
	Texture2D GetDefaultNormal() const { return _defaultNormal; }

	const auto& GetTextureVector() const { return _textures; }
	const auto& GetTextureIconVector() const { return _icons; }

	const fs::path* GetTexturePath(u32 textureID) const;

private:
	TexturesManager() = default;
	~TexturesManager() = default;

	// Default diffuse texture used when a mesh has no assigned diffuse texture.
	// This texture prevents unnecessary conditional checks in the rendering loop.
	// It is initialized as a 1x1 texture with default color (E.g. RGB: 128, 128, 255).
	Texture2D _defaultDiffuse;
	// It is initialized as a 1x1 texture with default values (E.g. RGB: 0, 0, 0).
	Texture2D _defaultSpecular;
	// It is initialized as a 1x1 texture with default values (E.g. RGB: 0, 0, 0).
	Texture2D _defaultNormal;

	// List of loaded textures 
	Vector<Texture2D> _textures;
	Vector<Texture2D> _icons;
	
	// Stores the file paths of loaded textures.
	// Each index in this vector corresponds to the same index in the `_textures` vector,
	// ensuring that `_texturePaths[i]` represents the file path of `_textures[i]`.
	Vector<fs::path> _texturePaths;
	Vector<fs::path> _iconPaths;
};
