#pragma once

#include "../Core.hh"

class ContentBrowserPanel
{
public:
	ContentBrowserPanel(const char* name, Vec2i size);
	~ContentBrowserPanel() = default;

	bool isOpen;
	
	String panelName;

	void RenderPanel();
	
private:
	Vec2i _panelSize;
	Path _currentDir;
	float _thumbPadding;
	float _thumbSize;

	class Texture2D* GetThumbIcon(const Path& entryPath, const char* entryStr, bool isDirectory) const;
};
