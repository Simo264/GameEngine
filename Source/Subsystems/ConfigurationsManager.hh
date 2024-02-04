#pragma once

#include "ResourceManager.hh"

#define CONF_WINDOW_TITLE				"window-title"
#define CONF_WINDOW_RESOLUTION	"window-resolution"
#define CONF_WINDOW_POS					"window-pos"
#define CONF_ASPECT_RATIO				"aspect-ratio"
#define CONF_VSYNC							"v-sync"

/* -----------------------------------------------------------------------
	Static class ConfigurationsManager:
	save and load the configuration of the application from file app.txt
 ----------------------------------------------------------------------- */
class ConfigurationsManager : public ResourceManagerInterface<ConfigurationsManager>
{
public:
	/* Initialize map */
	void Initialize() override;

	/* Release all objects the map has allocated internally */
	void ShutDown() override;

	/* Retrieve value from map by attrname */
	String& GetValue(const char* attrname) { return _mapConfig.at(attrname); }
	
	/* Set the value of attrname with attrvalue */
	void SetValue(const char* attrname, const char* attrvalue) { _mapConfig[attrname] = attrvalue; }

	/* Parse the string with the following format "<width>x<height>"; i.e. "1920x1080" */
	Vec2i ParseResolution(String& resolution);
	
	/* Parse the string with the following format "<posx>,<posy>"; i.e. "50,50" */
	Vec2i ParsePosition(String& position);
	
	/* Parse the string with the following format "<width>:<height>"; i.e. "16:9" */
	Vec2i ParseAspectRatio(String& aspect);

	/* Read "app.txt" file and load configuration */
	void LoadConfiguration();

	/* Save configuration in "app.txt" */
	void Save();

private:
	Path _configPath;
	Map<String, String> _mapConfig; /* <attrName, attrValue> */
};

