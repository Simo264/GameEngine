#pragma once

#include "../Core.hh"

class ContentBrowserPanel
{
public:
	ContentBrowserPanel(const char* panelName, Vec2i panelSize);
	~ContentBrowserPanel() = default;

	bool isOpen;

	void RenderPanel();
	
private:
	String _panelName;
	Vec2i _panelSize;

	Path _currentDir;
	float _thumbPadding;
	float _thumbSize;

	class Texture2D* GetThumbIcon(const Path& entryPath, const char* entryStr, bool isDirectory) const;
};
