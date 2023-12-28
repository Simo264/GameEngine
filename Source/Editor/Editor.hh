#pragma once

#include "../Core.hh"
#include "../Actor.hh"

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
	void ShowScenePanel(vector<Actor*>& sceneActors, DirectionalLight& dirLight);
	void ShowViewportPanel(const uint32_t& framebufferTexture);

private:
	bool _demoOpen = true;
	bool _scenePanelOpen = true;

	void Styling();
	void ShowPropertiesPanel(Actor* target);
};
