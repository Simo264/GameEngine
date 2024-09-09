#pragma once

#include "Core/Core.hpp"

String OpenFileDialog(int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);
String SaveFileDialog(int numFilters, const char* filter[], const char* filterDescription);