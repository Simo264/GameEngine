#pragma once

#include "Core/Core.hpp"

/**
 * @file FileDialogs.h
 * @brief Provides functions for opening and saving files through native dialogs.
 */

/**
 * @brief Opens a native file dialog to select files.
 *
 * This function displays an open file dialog, allowing the user to select one or multiple files
 * based on the specified filters and description.
 *
 * @param numFilters Number of filter patterns.
 * @param filter Array of filter patterns (e.g., {"*.txt", "*.png"}).
 * @param filterDescription Description for the filter (e.g., "Text Files").
 * @param multipleSelects Boolean flag indicating if multiple files can be selected.
 * @return A path containing the path of the selected file(s). If multiple files are selected,
 *         the paths are separated by '|'. Returns an empty String if the dialog is canceled.
 */
fs::path OpenFileDialog(i32 numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);

/**
 * @brief Opens a native file dialog to save a file.
 *
 * This function displays a save file dialog, allowing the user to specify the location
 * and name of the file to save, based on the provided filters and description.
 *
 * @param numFilters Number of filter patterns.
 * @param filter Array of filter patterns (e.g., {"*.txt", "*.png"}).
 * @param filterDescription Description for the filter (e.g., "Text Files").
 * @return A path containing the path of the file to be saved. Returns an empty String if the dialog is canceled.
 */
fs::path SaveFileDialog(i32 numFilters, const char* filter[], const char* filterDescription);