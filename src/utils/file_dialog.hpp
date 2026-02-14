#pragma once

#include "core/basic_types.hpp"
#include <filesystem>

/**
 * @brief Opens a file dialog to select one or multiple files.
 *
 * @param title The title of the dialog window.
 * @param default_path The initial directory or file suggestion.
 * @param num_filter_patterns The number of filter patterns provided.
 * @param filter_patterns An array of filter patterns (e.g., `*.txt`, `*.png`).
 * @param single_filter_description A description for the filter selection.
 * @param allow_multiple_selects Whether multiple file selections are allowed (true/false).
 * @return The selected file path as `fs::path`. Returns an empty path if no file is selected.
 */
std::filesystem::path open_file_dialog(const char* title,
                                       const std::filesystem::path default_path,
                                       i32 num_filter_patterns,
                                       const char* filter_patterns[],
                                       const char* single_filter_description,
                                       bool allow_multiple_selects);


/**
 * @brief Opens a file dialog to save a file.
 *
 * @param title The title of the dialog window.
 * @param default_path The initial directory or file suggestion.
 * @param num_filter_patterns The number of filter patterns provided.
 * @param filter_patterns An array of filter patterns (e.g., `*.txt`, `*.png`).
 * @param single_filter_description A description for the filter selection.
 * @return The selected file path as `fs::path`. Returns an empty path if no file is selected.
 */
std::filesystem::path save_file_dialog(const char* title,
                                       const std::filesystem::path default_path,
                                       i32 num_filter_patterns, 
                                       const char* filter_patterns[],
                                       const char* single_filter_description);
