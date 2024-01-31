#pragma once

#include "../Core.hh"

/* -----------------------------------------------------------------------
	Static class ConfigurationsManager:
	save and load the configuration of the application from file app.txt
 ----------------------------------------------------------------------- */
class ConfigurationsManager
{
public:
	ConfigurationsManager() = delete;
	~ConfigurationsManager() = delete;

	static void LoadConfiguration();
	static String& GetValue(const char* name) { return _mapConfig.at(name); }
	static void SetValue(const char* name, const char* value) { _mapConfig[name] = value; }

private:
	inline static Path _filePath = std::filesystem::current_path().parent_path() / "app.txt";
	inline static Map<String, String> _mapConfig = Map<String, String>(); /* <attrName, attrValue> */
};

