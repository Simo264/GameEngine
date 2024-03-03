#pragma once

#include "Editor/Panel.hpp"
#include "Core/FileParser/INIFileParser.hpp"

/* ----------------------------------------
			SettingsFrame class
	---------------------------------------- */
class SettingsPanel : public Panel
{
public:
	SettingsPanel(const char* panelName, bool visible);
	~SettingsPanel();

	void RenderFrame();

private:
	inline static const char* _aspectRatioValues[] = 
	{ 
		"21:9", 
		"16:9", 
		"4:3",
	};
	inline static const char* _resolutionValues[] = 
	{
		"2560x1080",	"3440x1440",	"3840x1600",	"5120x2160",	/* 21:9 resolutions */
		"1280x720",		"1366x768",		"1600x900",		"1920x1080",	/* 16:9 resolutions */
		"1280x960",		"1400x1050",	"1600x1200",	"1920x1440",	/* 4:3 resolutions */
	};

	int _aspectIndex;
	int _resolutionIndex;

	UniquePointer<INIFileParser> _conf;
	struct WindowArgs* _windowArgs;

	void OnSaveSettings();
};
