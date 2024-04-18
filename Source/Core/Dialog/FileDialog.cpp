#include "FileDialog.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

fspath FileDialog::OpenFileDialog(
	int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects)
{
	const char* filename = tinyfd_openFileDialog(
		"Tiny open file dialog",					/* title */
		ROOT_PATH.string().c_str(),				/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */
		filter,														/* aFilterPatterns */
		filterDescription,								/* aSingleFilterDescription */
		(int) multipleSelects							/* aAllowMultipleSelects */
	);

	if(filename)
		return fspath(filename);
	
	return fspath();
}

fspath FileDialog::SaveFileDialog(int numFilters, const char* filter[], const char* filterDescription)
{
	const char* filename = tinyfd_saveFileDialog(
		"Tiny save file dialog",					/* title */
		ROOT_PATH.string().c_str(),				/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */	
		filter,														/* aFilterPatterns */
		filterDescription									/* aSingleFilterDescription */
	);

	if (filename)
		return fspath(filename);

	return fspath();
}

