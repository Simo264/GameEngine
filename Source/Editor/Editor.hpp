#pragma once

#include "ContentBrowserPanel.hpp"
#include "ViewportPanel.hpp"
#include "OutlinerPanel.hpp"
#include "DetailsPanel.hpp"
#include "InspectorPanel.hpp"
#include "SettingsFrame.hpp"
#include "DebugFrame.hpp"
#include "MenuBar.hpp"

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
	void CleanUp();

	void Begin();
	void End();
	void Render(class Scene* scene, class FrameBuffer* framebuffer);

	UniquePointer<ContentBrowserPanel> contentBrowserPanel;
	UniquePointer<ViewportPanel> viewportPanel;
	UniquePointer<OutlinerPanel> outlinerPanel;
	UniquePointer<DetailsPanel> detailsPanel;
	UniquePointer<InspectorPanel> inspectorPanel;
	UniquePointer<SettingsFrame> settingsFrame;
	UniquePointer<DebugFrame> debugFrame;
	UniquePointer<MenuBar> menuBar;

private:
	bool _demoOpen;
	bool _firstLoop;
	
	bool _newScene;
	bool _saveScene;
	bool _saveAsScene;
	bool _openScene;
	bool _exit;

	void Styling();
	void Dockspace();

	void OnNewScene(class Scene* scene);
	void OnOpenScene(class Scene* scene);
	void OnSaveScene(class Scene* scene);
	void OnSaveAsScene(class Scene* scene);
};
