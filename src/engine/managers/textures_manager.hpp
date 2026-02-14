#pragma once

#include "Core/Types.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Functional.hpp"
#include "Core/Containers.hpp"
#include "Core/Singleton.hpp"
#include "Engine/Graphics/Texture2D.hpp"

/**
 * @brief Manages the loading, storage, and retrieval of textures and icons.
 * This class implements the singleton pattern to ensure only one instance of TextureManager
 * exists throughout the application.
 */
class TexturesManager : public Singleton<TexturesManager>
{
public:
	/** @brief Initializes the TextureManager by creating default textures and loading additional textures and icons */
	void Initialize();
	/** @brief Cleans up loaded textures and icons by deleting their OpenGL references. */
	void Cleanup();
	
	/** @brief Finds an existing texture inside "Assets/Textures" (e.g. "skybox/front.jpg") */
	Optional<Texture2D> FindTexture(const fs::path& relative) const;
	/** @brief Creates and loads a new texture inside "Assets/Textures" (e.g. "skybox/front.jpg") */
	Texture2D CreateTexture(const fs::path& relative);
	/** @brief Retrieves an existing texture or creates a new one if not found. */
	Texture2D GetOrCreateTexture(const fs::path& relative);
	/** @brief Finds an existing icon inside "Assets/Icons" (e.g. "back-arrow.png") */
	Optional<Texture2D> FindIcon(const fs::path& relative) const;
	/** @brief Creates and loads a new icon inside "Assets/Icons" (e.g. "back-arrow.png") */
	Texture2D CreateIcon(const fs::path& relative);
	/** @brief Retrieves an existing icon or creates a new one if not found. */
	Texture2D GetOrCreateIcon(const fs::path& relative);

	Optional<const fs::path*> GetTexturePath(TextureId textureID) const;

	Texture2D GetFallbackTexture() const { return _fallbackTexture; }
	auto& GetTextureVector() const { return _textures; }
	auto& GetTextureIconVector() const { return _icons; }

private:
	// Default texture used when a mesh has no assigned texture.
	// This texture prevents unnecessary conditional checks in the rendering loop.
	// It is initialized as a 1x1 texture with default color (E.g. RGB: 128, 128, 255).
	Texture2D _fallbackTexture;

	// List of loaded textures 
	Vector<Texture2D> _textures;
	Vector<Texture2D> _icons;
	
	// Stores the file paths of loaded textures.
	// Each index in this vector corresponds to the same index in the `_textures` vector,
	// ensuring that `_texturePaths[i]` represents the file path of `_textures[i]`.
	Vector<fs::path> _texturePaths;
	Vector<fs::path> _iconPaths;
};
