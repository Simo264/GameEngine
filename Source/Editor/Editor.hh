#pragma once

#include "ContentBrowserPanel.hh"
#include "ViewportPanel.hh"
#include "OutlinerPanel.hh"
#include "DetailsPanel.hh"
#include "InspectorPanel.hh"
#include "SettingsFrame.hh"
#include "DebugFrame.hh"
#include "MenuBar.hh"

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

	UniquePointer<ContentBrowserPanel> contentBrowserPanel;
	UniquePointer<ViewportPanel> viewportPanel;
	UniquePointer<OutlinerPanel> outlinerPanel;
	UniquePointer<DetailsPanel> detailsPanel;
	UniquePointer<InspectorPanel> inspectorPanel;
	UniquePointer<SettingsFrame> settingsFrame;
	UniquePointer<DebugFrame> debugFrame;
	UniquePointer<MenuBar> menuBar;

private:
	bool _demoOpen = true;
	bool _firstLoop = true;

	void Styling();
	void Dockspace() const;
};
