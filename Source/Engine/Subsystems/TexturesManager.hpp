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
	
	const Texture2D* FindTexture(const fs::path& path) const;
	const Texture2D* InsertTexture(const fs::path& path, bool gamma = false);
	
	const Texture2D* FindTextureIcon(const fs::path& path) const;
	const Texture2D* InsertTextureIcon(const fs::path& path);
	
	/** @brief Retrieves the default diffuse texture. */
	const Texture2D& GetDefaultDiffuse() const { return _textures.at(0); }
	/** @brief Retrieves the default specular  texture. */
	const Texture2D& GetDefaultSpecular() const { return _textures.at(1); }
	/** @brief Retrieves the default normal  texture. */
	const Texture2D& GetDefaultNormal() const { return _textures.at(2); }
	/** @brief Retrieves all loaded textures. */
	const auto& GetTextureVector() const { return _textures; }
	const auto& GetTextureIconVector() const { return _textures; }

private:
	TexturesManager() = default;
	~TexturesManager() = default;

	Vector<Texture2D> _textures;
	Vector<Texture2D> _icons;

	// Map storing texture path and its index
	UnorderedMap<fs::path, u32> _textureMap;
	UnorderedMap<fs::path, u32> _iconMap;

	void LoadAllTextures();
	void LoadAllTextureIcons();
};
