#pragma once

#include "Core.hh"

/* --------------------------------------------
		FileDialog static class 
	-------------------------------------------- */
class FileDialog
{
public:
	FileDialog() = delete;
	~FileDialog() = delete;

	/* Return empty path if cancelled */
	static Path OpenFileDialog(const char* filter);
	static Path SaveFileDialog(const char* filter);
};

