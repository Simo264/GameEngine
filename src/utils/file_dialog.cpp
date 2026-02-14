#include "file_dialog.hpp"

#include <tinyfiledialogs.h>

std::filesystem::path open_file_dialog(const char* title,
                                       const std::filesystem::path default_path,
                                       i32 num_filter_patterns,
                                       const char* filter_patterns[],
                                       const char* single_filter_description,
                                       bool allow_multiple_selects)
{
  auto filename = tinyfd_openFileDialog(title,
                                        default_path.string().c_str(),
                                        num_filter_patterns,
                                        filter_patterns,
                                        single_filter_description,
                                        static_cast<i32>(allow_multiple_selects));

  if (filename)
    return std::filesystem::path(filename);

  return std::filesystem::path();
}

std::filesystem::path save_file_dialog(const char* title,
                                       const fs::path default_path,
                                       i32 num_filter_patterns,
                                       const char* filter_patterns[],
                                       const char* single_filter_description)
{
  auto filename = tinyfd_saveFileDialog(title,
                                        default_path.string().c_str(),
                                        num_filter_patterns,
                                        filter_patterns,
                                        single_filter_description);

  if (filename)
    return std::filesystem::path(filename);

  return std::filesystem::path();
}