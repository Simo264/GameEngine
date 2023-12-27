#pragma once

#include "../Core.hh"
#include "../Actor.hh"
#include "../Lighting/DirectionalLight.hh"

class Editor
{
public:
	Editor()	= default;
	~Editor() = default;

	void Initialize();
	void ShutDown();

	void NewFrame();
	void RenderFrame();

	void ShowDemo();
	void ShowHelloWorld();
	void ShowScenePanel(vector<Actor*>& sceneActors, DirectionalLight& dirLight);

private:
	void Styling();
};
