#pragma once

#include "Core/Core.hpp"

/* --------------------------------------------
		FileDialog static class 
	-------------------------------------------- */
class FileDialog
{
public:
	FileDialog() = delete;
	~FileDialog() = delete;

	/* Return empty path if cancelled */
	static fs::path OpenFileDialog(
		i32 numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);
	
	static fs::path SaveFileDialog(
		i32 numFilters, const char* filter[], const char* filterDescription);
};

