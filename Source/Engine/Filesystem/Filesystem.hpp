#pragma once

#include "Core/Core.hpp"

/**
 * @namespace Filesystem
 * @brief Provides utility functions to retrieve various asset and resource paths within the project.
 *
 * This namespace contains functions that return the paths to different directories used in the engine,
 * including paths for assets, shaders, textures, fonts, and models.
 */
namespace Filesystem
{
	/** @brief Retrieves the root path of the project directory. */
	const fs::path& GetRootPath();

	/** @brief Retrieves the path to the assets directory. */
	const fs::path& GetAssetsPath();

	/** @brief Retrieves the path to the shaders directory. */
	const fs::path& GetShadersPath();

	/** @brief Retrieves the path to the textures directory. */
	const fs::path& GetTexturesPath();

	/** @brief Retrieves the path to the icons directory. */
	const fs::path& GetIconsPath();

	/** @brief Retrieves the path to the fonts directory. */
	const fs::path& GetFontsPath();

	/** @brief Retrieves the path to the models directory. */
	const fs::path& GetModelsPath();

	/** @brief Retrieves the path to the static models directory. */
	const fs::path& GetStaticModelsPath();
	
	/** @brief Retrieves the path to the skeletal models directory. */
	const fs::path& GetSkeletalModelsPath();
}