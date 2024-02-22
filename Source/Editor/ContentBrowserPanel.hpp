#pragma once

#include "Core.hpp"

/* ----------------------------------------
			ContentBrowserPanel class
	---------------------------------------- */
class ContentBrowserPanel
{
public:
	ContentBrowserPanel(const char* panelName);

	bool isOpen;
	String panelName;

	void RenderPanel();
	
private:
	Path _currentDir;
	float _thumbPadding;
	float _thumbSize;
	
	class Texture2D* GetThumbIcon(const Path& entryPath, const char* entryStr, bool isDirectory) const;
};
