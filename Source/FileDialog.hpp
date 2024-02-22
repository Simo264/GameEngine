#pragma once

#include "Core.hpp"

/* --------------------------------------------
		FileDialog static class 
	-------------------------------------------- */
class FileDialog
{
public:
	FileDialog() = delete;
	~FileDialog() = delete;

	/* Return empty path if cancelled */
	static Path OpenFileDialog(
		int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects);
	
	static Path SaveFileDialog(
		int numFilters, const char* filter[], const char* filterDescription);
};

