#include "FileDialog.hpp"

#include <tinyfiledialogs.h>

namespace FileDialog
{
	fs::path OpenFileDialog(const char* title,
													const fs::path defaultPath,
													i32 numOfFilterPatterns,
													const char* filterPatterns[],
													const char* singleFilterDescription,
													bool allowMultipleSelects)
	{
		const char* filename = tinyfd_openFileDialog(
			title,
			defaultPath.string().c_str(),
			numOfFilterPatterns,
			filterPatterns,
			singleFilterDescription,
			static_cast<i32>(allowMultipleSelects)
		);

		if (filename)
			return fs::path(filename);

		return fs::path();
	}

	fs::path SaveFileDialog(const char* title,
													const fs::path defaultPath,
													i32 numOfFilterPatterns,
													const char* filterPatterns[],
													const char* singleFilterDescription)
	{
		const char* filename = tinyfd_saveFileDialog(
			title,
			defaultPath.string().c_str(),
			numOfFilterPatterns,
			filterPatterns,
			singleFilterDescription
		);

		if (filename)
			return fs::path(filename);

		return fs::path();
	}
}
