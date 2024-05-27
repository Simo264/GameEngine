//#pragma once
//
//#include "Editor/ContentBrowserPanel.hpp"
//#include "Editor/ViewportPanel.hpp"
//#include "Editor/OutlinerPanel.hpp"
//#include "Editor/DetailsPanel.hpp"
//#include "Editor/InspectorPanel.hpp"
//#include "Editor/SettingsPanel.hpp"
//#include "Editor/DebugPanel.hpp"
//#include "Editor/MenuBar.hpp"
//
///* Editor layout 1600x900
//
//	--------------------------------------
//	|        |                  |        |
//	|        |                  |        |
//	|20%x100%|    60% x 60%     |20%x100%|
//	|        |    viewport      |        |
//	|        |                  |        |
//	|        |                  |        |
//	|        |------------------|        |
//	|        |    60% x 40%     |        |
//	|        | content browser  |        |
//	--------------------------------------
//*/
//
//class Editor
//{
//public:
//	Editor()	= default;
//	~Editor() = default;
//
//	void Initialize();
//	void CleanUp();
//
//	void Begin();
//	void End();
//
//	void Render(
//		class Scene& scene, 
//		class Camera& camera, 
//		const uint32_t framebufferImage);
//
//	unique_pointer<ContentBrowserPanel>	contentBrowserPanel;
//	unique_pointer<ViewportPanel>				viewportPanel;
//	unique_pointer<OutlinerPanel>				outlinerPanel;
//	unique_pointer<DetailsPanel>				detailsPanel;
//	unique_pointer<InspectorPanel>			inspectorPanel;
//	unique_pointer<SettingsPanel>				settingsPanel;
//	unique_pointer<DebugPanel>					debugPanel;
//	unique_pointer<MenuBar>							menuBar;
//
//private:
//	bool _demoOpen;
//	bool _firstLoop;
//	
//	bool _newScene;
//	bool _saveScene;
//	bool _saveAsScene;
//	bool _openScene;
//	bool _exit;
//
//	void Styling();
//	void Dockspace();
//
//	void OnNewScene(class Scene& scene);
//	void OnOpenScene(class Scene& scene);
//	//void OnSaveScene(class Scene& scene);
//	void OnSaveAsScene(class Scene& scene);
//};
