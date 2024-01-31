#include "ConfigurationsManager.hh"
#include "../Logger.hh"

void ConfigurationsManager::LoadConfiguration()
{
	IFStream file(_filePath.string());
	if (!file.is_open())
	{
		CONSOLE_ERROR("'{}' does not exist.", _filePath.string());
		exit(-1);
	}
	
	String line;
	while (std::getline(file, line)) 
	{
		/* Ignore comments */
		if (line[0] == '#') 
			continue;

		char name[64]{};
		char value[64]{};
		auto eq = line.find('=');
		std::copy(line.begin(), line.begin() + eq, name);
		std::copy(line.begin() + eq + 1, line.end(), value);

		_mapConfig.insert(std::make_pair(name, value));
	}

	file.close();
}
