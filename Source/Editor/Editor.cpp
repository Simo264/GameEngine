#include "Editor.hh"
#include "ContentBrowserPanel.hh"

#include "../FrameBuffer.hh"
#include "../Scene.hh"
#include "../Subsystems/ConfigurationsManager.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

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
  settingsFrame = std::make_unique<SettingsFrame>("Settings");
  debugFrame = std::make_unique<DebugFrame>("Debug");

  Array<bool*, 7> items{};
  items[PANEL_BROWSER] = &contentBrowserPanel->isOpen;
  items[PANEL_INSPECTOR] = &inspectorPanel->isOpen;
  items[PANEL_OUTLINER] = &outlinerPanel->isOpen;
  items[PANEL_VIEWPORT] = &viewportPanel->isOpen;
  items[FRAME_DEBUG] = &debugFrame->isOpen;
  items[FRAME_DEMO] = &_demoOpen;
  items[FRAME_SETTINGS] = &settingsFrame->isOpen;

  menuBar = std::make_unique<MenuBar>(items);
}

void Editor::ShutDown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Editor::NewFrame()
{
  /* Start the Dear ImGui frame */
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  /* Set Dockspace */
  //Dockspace();
  
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

      auto dockOutliner   = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left,  0.20f, nullptr, &dockspaceID);
      auto dockInspector  = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.25f, nullptr, &dockspaceID);
      auto dockBrowser    = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down,  0.40f, nullptr, &dockspaceID);
      auto dockViewport   = ImGui::DockBuilderGetCentralNode(dockspaceID)->ID;
      auto dockDemo       = ImGui::DockBuilderSplitNode(dockInspector, ImGuiDir_Down, 0.60f, nullptr, &dockInspector);
      auto dockDetails    = ImGui::DockBuilderSplitNode(dockOutliner, ImGuiDir_Down, 0.60f, nullptr, &dockOutliner);

      ImGui::DockBuilderDockWindow(outlinerPanel->panelName.c_str(), dockOutliner);
      ImGui::DockBuilderDockWindow(inspectorPanel->panelName.c_str(), dockInspector);
      ImGui::DockBuilderDockWindow(contentBrowserPanel->panelName.c_str(), dockBrowser);
      ImGui::DockBuilderDockWindow(viewportPanel->panelName.c_str(), dockViewport);
      ImGui::DockBuilderDockWindow(detailsPanel->panelName.c_str(), dockDetails);
      ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockDemo);
      ImGui::DockBuilderFinish(dockspaceID);
    }
  }

  /* Menu bar here */
  menuBar->RenderMenuBar();

  ImGui::End();
}

void Editor::RenderEditor(Scene* scene, FrameBuffer* framebuffer)
{
  if (_demoOpen) 
    ImGui::ShowDemoWindow(&_demoOpen);

  if (settingsFrame->isOpen)
    settingsFrame->RenderFrame();

  if (debugFrame->isOpen)
    debugFrame->RenderFrame();

  if (contentBrowserPanel->isOpen)
    contentBrowserPanel->RenderPanel();

  if(viewportPanel->isOpen)
    viewportPanel->RenderPanel(framebuffer);
  
  if(outlinerPanel->isOpen)
    outlinerPanel->RenderPanel(scene);

  if (outlinerPanel->IsItemSelected())
  {
    auto dLight = outlinerPanel->GetItemSelected<DirectionalLight>();
    auto pLight = outlinerPanel->GetItemSelected<PointLight>();
    auto sMesh = outlinerPanel->GetItemSelected<StaticMesh>();
    if(dLight)
      detailsPanel->RenderPanel(*dLight);
    else if(pLight)
      detailsPanel->RenderPanel(*pLight);
    else if(sMesh)
      detailsPanel->RenderPanel(*sMesh);
  }
  
  if(inspectorPanel->isOpen)
    inspectorPanel->RenderPanel();

  ImGui::RenderPanel();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  /* Update and Render additional Platform Windows (Platform functions may change 
  the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  For this specific demo app we could also call glfwMakeContextCurrent(window) directly) */
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
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
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); // custom font

  ImGui::StyleColorsDark();
}

void Editor::Dockspace() const
{
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  /* We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
   because it would be confusing to have two docking targets within each others. */
  const ImGuiViewport* imViewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(imViewport->WorkPos);
  ImGui::SetNextWindowSize(imViewport->WorkSize);
  ImGui::SetNextWindowViewport(imViewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


  /* When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
  and handle the pass-thru hole, so we ask Begin() to not render a background.*/
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  /* Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  all active windows docked into it will lose their parent and become undocked.
  We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
  any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.*/
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  
  ImGui::Begin("DockSpace Demo", nullptr, window_flags);
  
  ImGui::PopStyleVar();
  ImGui::PopStyleVar(2);

  /* Submit the DockSpace */
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }

  menuBar->RenderMenuBar();
  
  ImGui::End();
}
