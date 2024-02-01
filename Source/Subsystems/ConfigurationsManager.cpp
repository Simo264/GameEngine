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
	char attrName[64]{};
	char attrVal[64]{};
	while (std::getline(file, line)) 
	{
		/* Ignore blank line */
		if (line.empty())
			continue;

		/* Ignore comments */
		if (line[0] == '#') 
			continue;
		
		auto sep = line.find('=');
		std::copy(line.begin(), line.begin() + sep, attrName);
		std::copy(line.begin() + sep + 1, line.end(), attrVal);
		
		_mapConfig.insert(std::make_pair(attrName, attrVal));

		std::fill_n(attrName, 64, 0);
		std::fill_n(attrVal, 64, 0);
	}

	file.close();
}

void ConfigurationsManager::Save()
{
	IFStream inFile(_filePath.string());
	
	/* Save file content in temporary buffer */
	StringStream buffer;
	buffer << inFile.rdbuf();
	inFile.close();

	/* Save new configurations */
	OFStream outFile(_filePath.string());
	String line;
	char attrName[64]{};
	while (std::getline(buffer, line)) 
	{
		if (line.empty())
		{
			outFile << "\n";
			continue;
		}
		if (line[0] == '#')
		{
			outFile << line;
			outFile << "\n";
			continue;
		}
		
		auto sep = line.find('=');
		std::copy(line.begin(), line.begin() + sep, attrName);

		outFile << attrName;
		outFile << "=";
		outFile << _mapConfig.at(attrName);
		outFile << "\n";

		std::fill_n(attrName, 64, 0);
	}

	outFile.close();
}
