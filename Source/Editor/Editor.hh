#pragma once

#include "ContentBrowserPanel.hh"
#include "ViewportPanel.hh"
#include "HierarchyPanel.hh"

/* Editor layout 1600x900

	--------------------------------------
	|        |                  |        |
	|        |                  |        |
	|20%x100%|    60% x 60%     |20%x100%|
	|        |    viewport      |        |
	|        |                  |        |
	|        |                  |        |
	|        |------------------|        |
	|        |    60% x 40%     |        |
	|        | content browser  |        |
	--------------------------------------
*/

class Editor
{
public:
	Editor()	= default;
	~Editor() = default;

	void Initialize();
	void ShutDown();

	void NewFrame();
	void RenderEditor(class Scene* scene, class FrameBuffer* framebuffer);

	UniquePointer<ContentBrowserPanel> contentBrowser;
	UniquePointer<ViewportPanel> viewport;
	UniquePointer<HierarchyPanel> hierarchy;

private:
	bool _preferencesOpen = false;

	bool _demoOpen = true;
	bool _statsOpen = true;
	bool _inspectorOpen = true;

	Vec2i _inspectorSize; 

	inline static const char* aspectRatioValues[] = { "21:9", "16:9", "4:3" };
	inline static const char* resolutionValues[] = { 
		"1280x960", "1400x1050", "1600x1200", "1920x1440",	/* 4:3 resolutions */
		"1280x720", "1366x768", "1600x900", "1920x1080",		/* 16:9 resolutions */
		"2560x1080", "3440x1440", "3840x1600", "5120x2160",	/* 21:9 resolutions */
	};

	int _aspectIndex;
	int _resolutionIndex;
	
	void Styling();
	void Dockspace();

	void ShowStats();
	void ShowInspector(); /* TODO */

	void ShowPreferences();
};
