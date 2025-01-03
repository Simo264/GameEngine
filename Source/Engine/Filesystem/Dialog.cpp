#include "Dialog.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

fs::path OpenFileDialog(i32 numFilters, const char* filter[], const char* filterDescription, bool multipleSelects)
{
	// Calls tinyfd_openFileDialog from tinyfiledialogs to open a file selection dialog.
	// If multipleSelects is true, the user can select multiple files.
	// Returns the selected file(s) path(s) or an empty String if canceled.
	const char* filename = tinyfd_openFileDialog(
		"Open file dialog",			// Title of the dialog window
		nullptr,								// Default path and file (nullptr uses current directory)
		numFilters,							// Number of filter patterns provided
		filter,									// Array of filter patterns (e.g., *.txt, *.png)
		filterDescription,			// Description of the selected filter type
		(i32)multipleSelects		// Allow multiple file selections (1 = true, 0 = false)
	);

	if (filename)
		return fs::path(filename);

	return fs::path();
}

fs::path SaveFileDialog(i32 numFilters, const char* filter[], const char* filterDescription)
{
	// Calls tinyfd_saveFileDialog from tinyfiledialogs to open a save file dialog.
	// The user can specify the name and location of the file to save.
	// Returns the selected file path or an empty String if canceled.
	const char* filename = tinyfd_saveFileDialog(
		"Save file dialog",		// Title of the dialog window
		nullptr,							// Default path and file (nullptr uses current directory)
		numFilters,						// Number of filter patterns provided
		filter,								// Array of filter patterns (e.g., *.txt, *.png)
		filterDescription			// Description of the selected filter type
	);

	if (filename)
		return fs::path(filename);

	return fs::path();
}