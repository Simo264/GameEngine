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
	
	// <path> relative path to "Assets/Textures"
	const Texture2D* FindTexture(const fs::path& path) const;
	
	// <path> relative path to "Assets/Textures"
	const Texture2D* CreateTexture(const fs::path& path);
	
	// <path> relative path to "Assets/Icons"
	const Texture2D* GetOrCreateTexture(const fs::path& path);

	// <path> relative path to "Assets/Icons"
	const Texture2D* FindIcon(const fs::path& path) const;

	// <path> relative path to "Assets/Icons"
	const Texture2D* CreateIcon(const fs::path& path);
	
	// <path> relative path to "Assets/Icons"
	const Texture2D* GetOrCreateIcon(const fs::path& path);
	
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
	void LoadAllIcons();
};
