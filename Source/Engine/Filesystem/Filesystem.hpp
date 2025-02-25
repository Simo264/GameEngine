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
	/** @brief Retrieves the absolute path to directory of the project. */
	const fs::path& GetRootPath();
	const fs::path& GetAssetsPath();
	const fs::path& GetShadersPath();
	const fs::path& GetTexturesPath();
	const fs::path& GetIconsPath();
	const fs::path& GetFontsPath();
	const fs::path& GetModelsPath();
	const fs::path& GetStaticModelsPath();
	const fs::path& GetSkeletalModelsPath();
}