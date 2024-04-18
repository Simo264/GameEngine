#pragma once

#include "Core/Core.hpp"
#include "Engine/Subsystems/Manager.hpp"
#include "Engine/Graphics/Texture2D.hpp"

/**
 * Load and retrieve texture objects from the array
 */
class TextureManager : public Manager<TextureManager>
{
public:
	/**
	 * Initialize the texture array and load images from "Textures/" and "Icons/" directories
	 */
	void Initialize() override;
	
	/**
	 * Destroy all texture objects 
	 */
	void CleanUp() override;

	/**
	 * Load texture object in array
	 */
	Texture2D* LoadTexture(const fspath& filePath, bool gammaCorrection = true);
		
	/**
	 * Retrieve texture object from pool
	 */
	Texture2D* GetTextureByPath(const fspath& filePath);

	constexpr auto Begin() const { return textures.begin(); }

	constexpr auto End() const { return textures.end(); }

	constexpr auto Size() const { return textures.size(); }

	vector<Texture2D> textures;
};
