#pragma once

#include "Core/Core.hpp"

fs::path OpenFileDialog(int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);
fs::path SaveFileDialog(int numFilters, const char* filter[], const char* filterDescription);