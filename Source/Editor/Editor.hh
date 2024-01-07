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
	void RenderFrame();

	void MenuBar();

	void ShowDemo();
	void ShowStats();
	void ShowScenePanel(class Scene* scene);
	//void ShowViewportPanel(const uint32_t& framebufferTexture);

private:
	bool _demoPanelOpen  = true;
	bool _scenePanelOpen = true;
	bool _statsPanelOpen = true;

	void Styling();
	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);
};
