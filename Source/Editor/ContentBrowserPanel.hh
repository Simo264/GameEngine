#pragma once

#include "../Core.hh"

/* ----------------------------------------
			ContentBrowserPanel class
	---------------------------------------- */
class ContentBrowserPanel
{
public:
	ContentBrowserPanel(const char* panelName);
	~ContentBrowserPanel() = default;

	bool isOpen;
	String panelName;

	void RenderPanel();
	
private:
	Path _currentDir;
	float _thumbPadding;
	float _thumbSize;
	
	class Texture2D* GetThumbIcon(const Path& entryPath, const char* entryStr, bool isDirectory) const;
};
