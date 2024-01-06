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
	void ShowScenePanel(class DirectionalLight* sceneLight, Vector<class StaticMesh*>& sceneMeshes);
	//void ShowViewportPanel(const uint32_t& framebufferTexture);

private:
	bool _demoPanelOpen  = true;
	bool _scenePanelOpen = true;
	bool _statsPanelOpen = true;

	void Styling();
	void ShowPropertiesPanel(class StaticMesh* meshTarget);
};
