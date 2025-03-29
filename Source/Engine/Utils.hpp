#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

namespace Utils
{
	/**
	 * @brief Counts the number of files in a directory.
	 *
	 * This function counts the number of files in the specified directory. If the recursive flag is set to true,
	 * it will count the files in all subdirectories as well.
	 */
	u32 CountFilesInDirectory(const fs::path &directoryPath, bool recursive = false);
};
