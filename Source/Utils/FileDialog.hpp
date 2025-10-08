#pragma once

#include "Core/Types.hpp"
#include "Core/FileSystem.hpp"

namespace FileDialog
{
	/**
	 * @brief Opens a file dialog to select one or multiple files.
	 *
	 * @param title The title of the dialog window.
	 * @param defaultPath The initial directory or file suggestion.
	 * @param numOfFilterPatterns The number of filter patterns provided.
	 * @param filterPatterns An array of filter patterns (e.g., `*.txt`, `*.png`).
	 * @param singleFilterDescription A description for the filter selection.
	 * @param allowMultipleSelects Whether multiple file selections are allowed (true/false).
	 * @return The selected file path as `fs::path`. Returns an empty path if no file is selected.
	 */
	fs::path OpenFileDialog(const char* title,
													const fs::path defaultPath,
													i32 numOfFilterPatterns,
													const char* filterPatterns[],
													const char* singleFilterDescription,
													bool allowMultipleSelects);


	/**
	 * @brief Opens a file dialog to save a file.
	 *
	 * @param title The title of the dialog window.
	 * @param defaultPath The initial directory or file suggestion.
	 * @param numOfFilterPatterns The number of filter patterns provided.
	 * @param filterPatterns An array of filter patterns (e.g., `*.txt`, `*.png`).
	 * @param singleFilterDescription A description for the filter selection.
	 * @return The selected file path as `fs::path`. Returns an empty path if no file is selected.
	 */
	fs::path SaveFileDialog(const char* title,
													const fs::path defaultPath,
													i32 numOfFilterPatterns, 
													const char* filterPatterns[],
													const char* singleFilterDescription);
}
