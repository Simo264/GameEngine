#include "Editor.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"
#include "Engine/StaticMesh.hpp"

#include "INIFileManager.hpp"
#include "FileDialog.hpp"
#include "Logger.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>


/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Editor::Initialize()
{
  /* Init ImGui */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       /* Enable Multi-Viewport / Platform Windows */
  
  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  /* Set ImGui style*/
  Styling();

  contentBrowserPanel = std::make_unique<ContentBrowserPanel>("Content Browser");
  viewportPanel = std::make_unique<ViewportPanel>("Viewport");
  outlinerPanel = std::make_unique<OutlinerPanel>("Outliner");
  detailsPanel = std::make_unique<DetailsPanel>("Details");
  inspectorPanel = std::make_unique<InspectorPanel>("Inspector");
  settingsFrame = std::make_unique<SettingsFrame>();
  debugFrame = std::make_unique<DebugFrame>("Debug");

  _demoOpen = true;
  _firstLoop = true;
  _newScene = false;
  _saveScene = false;
  _saveAsScene = false;
  _openScene = false;
  _exit = false;

  Array<bool*, MENU_ITEMS_COUNT> items{};
  items[PANEL_BROWSER] = &contentBrowserPanel->isOpen;
  items[PANEL_INSPECTOR] = &inspectorPanel->isOpen;
  items[PANEL_OUTLINER] = &outlinerPanel->isOpen;
  items[PANEL_VIEWPORT] = &viewportPanel->isOpen;
  items[PANEL_DETAILS] = &detailsPanel->isOpen;
  items[FRAME_DEBUG] = &debugFrame->isOpen;
  items[FRAME_DEMO] = &_demoOpen;
  items[FRAME_SETTINGS] = &settingsFrame->isOpen;
  items[NEW_SCENE] = &_newScene;
  items[SAVE_SCENE] = &_saveScene;
  items[SAVE_AS_SCENE] = &_saveAsScene;
  items[OPEN_SCENE] = &_openScene;
  items[EXIT] = &_exit;

  menuBar = std::make_unique<MenuBar>(items);
}

void Editor::ShutDown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Editor::Begin()
{
  /* Start the Dear ImGui frame */
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::Begin();
  ImGuizmo::BeginFrame();

  /* Set Dockspace */
  Dockspace();
}

void Editor::End()
{
  ImGui::RenderPanel();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void Editor::Render(Scene* scene, FrameBuffer* framebuffer)
{
  if (_demoOpen) 
    ImGui::ShowDemoWindow(&_demoOpen);

  if (settingsFrame->isOpen)
    settingsFrame->RenderFrame();

  if (debugFrame->isOpen)
    debugFrame->RenderFrame();

  if (contentBrowserPanel->isOpen)
    contentBrowserPanel->RenderPanel();
  
  if(outlinerPanel->isOpen)
    outlinerPanel->RenderPanel(scene);

  auto& dLight = outlinerPanel->GetItemSelected<DirectionalLight>();
  auto& pLight = outlinerPanel->GetItemSelected<PointLight>();
  auto& sMesh  = outlinerPanel->GetItemSelected<StaticMesh>();

  if (outlinerPanel->IsItemSelected())
  {
    if (dLight)
      detailsPanel->RenderPanel(dLight, scene);
    else if (pLight)
      detailsPanel->RenderPanel(pLight, scene);
    else if (sMesh)
      detailsPanel->RenderPanel(sMesh, scene);
  }

  if (viewportPanel->isOpen)
    viewportPanel->RenderPanel(framebuffer, sMesh.get());
  
  if(inspectorPanel->isOpen)
    inspectorPanel->RenderPanel();

  if (_newScene)
    OnNewScene(scene);
  else if (_saveAsScene)
    OnSaveAsScene(scene);
  else if (_saveScene)
    OnSaveScene(scene);
  else if (_openScene)
    OnOpenScene(scene);

  if (_exit)
  {
    CONSOLE_INFO("Exit...");
    _exit = false;
  }
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Editor::Styling()
{
  Path fontPath = ROOT_PATH / "Fonts/Karla-Regular.ttf";
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); 

  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding = ImVec2(8.00f, 8.00f);
  style.FramePadding = ImVec2(5.00f, 2.00f);
  style.CellPadding = ImVec2(6.00f, 6.00f);
  style.ItemSpacing = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
  style.IndentSpacing = 25;
  style.ScrollbarSize = 15;
  style.GrabMinSize = 10;
  style.WindowBorderSize = 1;
  style.ChildBorderSize = 1;
  style.PopupBorderSize = 1;
  style.FrameBorderSize = 1;
  style.TabBorderSize = 1;
  style.WindowRounding = 7;
  style.ChildRounding = 4;
  style.FrameRounding = 3;
  style.PopupRounding = 4;
  style.ScrollbarRounding = 9;
  style.GrabRounding = 3;
  style.LogSliderDeadzone = 4;
  style.TabRounding = 4;
}

void Editor::Dockspace()
{
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
  windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

  ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  ImGui::Begin("DockSpace", nullptr, windowFlags);
  {
    ImGui::PopStyleVar(3);
    ImGuiID dockspaceID = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

    if (_firstLoop)
    {
      _firstLoop = false;

      ImGui::DockBuilderRemoveNode(dockspaceID);   /* clear any previous layout */
      ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

      auto dockOutliner = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.20f, nullptr, &dockspaceID);
      auto dockInspector = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.25f, nullptr, &dockspaceID);
      auto dockBrowser = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.40f, nullptr, &dockspaceID);
      auto dockViewport = ImGui::DockBuilderGetCentralNode(dockspaceID)->ID;
      auto dockDemo = ImGui::DockBuilderSplitNode(dockInspector, ImGuiDir_Down, 0.60f, nullptr, &dockInspector);
      auto dockDetails = ImGui::DockBuilderSplitNode(dockOutliner, ImGuiDir_Down, 0.60f, nullptr, &dockOutliner);
      
      ImGui::DockBuilderDockWindow(outlinerPanel->panelName.c_str(), dockOutliner); /* Outliner panel */
      ImGui::DockBuilderDockWindow(inspectorPanel->panelName.c_str(), dockInspector); /* Inspector panel */
      ImGui::DockBuilderDockWindow(contentBrowserPanel->panelName.c_str(), dockBrowser); /* Browser panel */
      ImGui::DockBuilderDockWindow(viewportPanel->panelName.c_str(), dockViewport); /* Viewport panel */
      ImGui::DockBuilderDockWindow(detailsPanel->panelName.c_str(), dockDetails); /* Details panel */
      ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockDemo); /* Demo panel*/
      ImGui::DockBuilderFinish(dockspaceID);
    }
  }

  /* Menu bar here */
  menuBar->RenderMenuBar();

  ImGui::End();
}

void Editor::OnNewScene(Scene* scene)
{
  _newScene = false;
  scene->ClearScene();
}

void Editor::OnOpenScene(Scene* scene)
{
  _openScene = false;
  const char* filters[1] = { "*.ini" };
  Path filePath = FileDialog::OpenFileDialog(1, filters, "Open scene (.ini)", false);
  
  if (!filePath.empty())
  {
    scene->ClearScene();
    scene->LoadScene(filePath);
  }
}

void Editor::OnSaveScene(Scene* scene)
{
  _saveScene = false;
  CONSOLE_INFO("TODO OnSaveScene");
}

void Editor::OnSaveAsScene(Scene* scene)
{
  _saveAsScene = false;
  const char* filters[1] = { "*.ini" };
  Path filePath = FileDialog::SaveFileDialog(1, filters, "Save scene (.ini)");
  if (!filePath.empty())
    scene->SaveScene(filePath);
}
