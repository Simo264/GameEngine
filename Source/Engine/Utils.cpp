#include "Utils.hpp"

#include "Core/Log/Logger.hpp"

namespace Utils
{
	u32 CountFilesInDirectory(const fs::path &directoryPath, bool recursive)
	{
		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			CONSOLE_ERROR("Invalid directory {}", directoryPath.string());
			return 0;
		}

		auto fileCount = 0u;
		if (recursive)
		{
			for (const auto &entry : fs::recursive_directory_iterator(directoryPath))
				if (fs::is_regular_file(entry.status()))
					++fileCount;
		}
		else
		{
			for (const auto &entry : fs::directory_iterator(directoryPath))
				if (fs::is_regular_file(entry.status()))
					++fileCount;
		}

		return fileCount;
	}
};