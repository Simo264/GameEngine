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

	Texture2D* GetTextureByPath(const fs::path& filePath);
	Texture2D* GetTextureAt(i32 i) { return &_textures.at(i); }
	
	Texture2D* GetIconByPath(const fs::path& filePath);
	Texture2D* GetIconAt(i32 i) { return &_icons.at(i); }

	constexpr Vector<Texture2D>& GetTextureVector() { return _textures; }
	constexpr Vector<Texture2D>& GetIconVector() { return _icons; }

private:
	TextureManager() = default;
	~TextureManager() = default;

	Vector<Texture2D> _textures;
	Vector<Texture2D> _icons;

	void LoadTextures();
	void LoadIcons();
	void CreateDefaultTexture(Texture2D& texture, Array<u8, 3> textureData, StringView defaultPath);
};
