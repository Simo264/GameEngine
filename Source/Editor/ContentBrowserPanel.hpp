#pragma once

#include "Editor/Panel.hpp"

/* ----------------------------------------
			ContentBrowserPanel class
	---------------------------------------- */
class ContentBrowserPanel : public Panel
{
public:
	ContentBrowserPanel(const char* panelName, bool visible);

	void RenderPanel();
	
private:
	fspath _currentDir;
	float _thumbPadding;
	float _thumbSize;
	
	class Texture2D* GetThumbIcon(const fspath& entryPath, const char* entryStr, bool isDirectory) const;
};
