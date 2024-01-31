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
	void DebugValues(float* nearPlane, float* farPlane, Vec3f& lightPos);
	void ShowStats();
	void ShowHierarchy(class Scene* scene);
	void ShowViewport(const uint32_t& framebufferTexture) const;
	void ShowBrowser() const; /* TODO */
	void ShowInspector(); /* TODO */

private:
	bool _demoPanelOpen  = true;
	bool _statsOpen = true;
	bool _viewportOpen = true;
	bool _hierarchyOpen = true;
	bool _browserOpen = true;

	void Styling();
	void Dockspace();
	void ShowPropertiesPanel(class StaticMesh* meshTarget);
	void ShowPropertiesPanel(class DirectionalLight* dirLight);
	void ShowPropertiesPanel(class PointLight* pointLight);
};
