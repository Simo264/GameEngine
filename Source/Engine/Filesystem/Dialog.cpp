#include "Dialog.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

String OpenFileDialog(int numFilters, const char* filter[], const char* filterDescription, bool multipleSelects)
{
	const char* filename = tinyfd_openFileDialog(
		"Open file dialog",								/* title */
		nullptr,													/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */
		filter,														/* aFilterPatterns */
		filterDescription,								/* aSingleFilterDescription */
		(int)multipleSelects							/* aAllowMultipleSelects */
	);

	if (filename)
		return String(filename);

	return String();
}

String SaveFileDialog(int numFilters, const char* filter[], const char* filterDescription)
{
	const char* filename = tinyfd_saveFileDialog(
		"Save file dialog",								/* title */
		nullptr,													/* aDefaultPathAndFile */
		numFilters,												/* aNumOfFilterPatterns */
		filter,														/* aFilterPatterns */
		filterDescription									/* aSingleFilterDescription */
	);

	if (filename)
		return String(filename);

	return String();
}