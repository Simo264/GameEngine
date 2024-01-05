#pragma once

#include "../Core.hh"
#include "../Lighting/BaseLight.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"

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
	//void ShowScenePanel(DirectionalLight& light, Model& model);
	//void ShowViewportPanel(const uint32_t& framebufferTexture);

private:
	bool _demoPanelOpen  = true;
	bool _scenePanelOpen = true;
	bool _statsPanelOpen = true;

	void Styling();
	void ShowPropertiesPanel();
};
