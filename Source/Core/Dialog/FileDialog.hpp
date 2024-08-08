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
		int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);
	
	static fs::path SaveFileDialog(
		int numFilters, const char* filter[], const char* filterDescription);
};

