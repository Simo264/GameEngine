#pragma once

#include "../Core.hh"

class Editor
{
public:
	Editor()	= default;
	~Editor() = default;

	void Initialize();
	void ShutDown();

	void NewFrame();
	void RenderFrame(class Scene* scene, class FrameBuffer* framebuffer);

private:
	bool _demoOpen  = false;
	bool _statsOpen = false;
	bool _propertiesOpen = false;
	bool _preferencesOpen = false;

	bool _viewportOpen = true;
	bool _hierarchyOpen = true;
	bool _browserOpen = true;
	bool _inspectorOpen = true;
	
	inline static const char* aspectRatioValues[] = { "21:9", "16:9", "4:3" };
	inline static const char* resolutionValues[] = { 
		"1280x960", "1400x1050", "1600x1200", "1920x1440",	/* 4:3 resolutions */
		"1280x720", "1366x768", "1600x900", "1920x1080",		/* 16:9 resolutions */
		"2560x1080", "3440x1440", "3840x1600", "5120x2160",	/* 21:9 resolutions */
	};

	int aspectIndex;
	int resolutionIndex;

	
	void Styling();
	void Dockspace();

	void ShowStats();
	void ShowHierarchy(class Scene* scene);
	void ShowViewport(class FrameBuffer* framebuffer);
	void ShowBrowser(); /* TODO */
	void ShowInspector(); /* TODO */

	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);
	void ShowPreferences();
};
