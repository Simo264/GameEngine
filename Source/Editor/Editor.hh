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
	void RenderFrame(class Scene* scene, uint32_t framebufferTexture);

private:
	bool _demoOpen  = false;
	bool _statsOpen = false;
	bool _propertiesOpen = false;
	bool _preferencesOpen = false;

	bool _viewportOpen = true;
	bool _hierarchyOpen = true;
	bool _browserOpen = true;
	bool _inspectorOpen = true;
	
	void Styling();
	void Dockspace();

	void ShowStats();
	void ShowHierarchy(class Scene* scene);
	void ShowViewport(uint32_t framebufferTexture);
	void ShowBrowser(); /* TODO */
	void ShowInspector(); /* TODO */

	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);
	void ShowPreferences();
};
