#include "FileDialog.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

fs::path FileDialog::OpenFileDialog(
	i32 numFilters, const char* filter[], const char* filterDescription, bool multipleSelects)
{
	const char* filename = tinyfd_openFileDialog(
		"Tiny open file dialog",					/* title */
		ROOT_PATH.string().c_str(),				/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */
		filter,														/* aFilterPatterns */
		filterDescription,								/* aSingleFilterDescription */
		(i32) multipleSelects							/* aAllowMultipleSelects */
	);

	if(filename)
		return fs::path(filename);
	
	return fs::path();
}

fs::path FileDialog::SaveFileDialog(i32 numFilters, const char* filter[], const char* filterDescription)
{
	const char* filename = tinyfd_saveFileDialog(
		"Tiny save file dialog",					/* title */
		ROOT_PATH.string().c_str(),				/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */	
		filter,														/* aFilterPatterns */
		filterDescription									/* aSingleFilterDescription */
	);

	if (filename)
		return fs::path(filename);

	return fs::path();
}

