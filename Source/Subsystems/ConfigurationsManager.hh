#pragma once

#include "../Core.hh"

#define CONF_WINDOW_TITLE				"window-title"
#define CONF_WINDOW_RESOLUTION	"window-resolution"
#define CONF_WINDOW_POS					"window-pos"
#define CONF_ASPECT_RATIO				"aspect-ratio"
#define CONF_VSYNC							"v-sync"

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

	static Vec2i ParseResolution(String& resolution);
	static Vec2i ParsePosition(String& position);
	static Vec2i ParseAspectRatio(String& aspect);

	static void Save();

private:
	inline static Path _filePath = ROOT_PATH / "app.txt";
	inline static Map<String, String> _mapConfig = Map<String, String>(); /* <attrName, attrValue> */
};

