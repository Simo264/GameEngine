#include "Editor.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Mesh/StaticMesh.hh"
#include "../FrameBuffer.hh"
#include "../Scene.hh"

#include "../Subsystems/TexturesManager.hh"
#include "../Subsystems/ConfigurationsManager.hh"

#include "../Renderer.hh"
#include "../Logger.hh"

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
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  
  /* Set ImGui style*/
  Styling();

  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  auto& instanceCM = ConfigurationsManager::Instance();
  String& resolutionStr = instanceCM.GetValue(CONF_WINDOW_RESOLUTION);
  String& aspectRatioStr = instanceCM.GetValue(CONF_ASPECT_RATIO);
  for (_aspectIndex = 0; _aspectIndex < 3; _aspectIndex++)
    if (std::strcmp(_aspectRatioValues[_aspectIndex], aspectRatioStr.c_str()) == 0)
      break;

  for (_resolutionIndex = 0; _resolutionIndex < 12; _resolutionIndex++)
    if (std::strcmp(_resolutionValues[_resolutionIndex], resolutionStr.c_str()) == 0)
      break;

  Vec2i resolution = instanceCM.ParseResolution(resolutionStr);

  Vec2i viewportPanelSize  = Vec2i(resolution.x * 0.6, resolution.y * 0.6); /* 60% x 60%  */
  Vec2i scenePanelSize = Vec2i(resolution.x * 0.2, resolution.y * 1);   /* 20% x 100% */
  _inspectorSize = Vec2i(resolution.x * 0.2, resolution.y * 1);   /* 20% x 100% */
  Vec2i browserPanelSize = Vec2i(resolution.x * 0.6, resolution.y * 0.4); /* 60% x 40%  */

  contentBrowserPanel = std::make_unique<ContentBrowserPanel>("Content Browser", browserPanelSize);
  viewportPanel = std::make_unique<ViewportPanel>("Viewport", viewportPanelSize);
  scenePanel = std::make_unique<ScenePanel>("Scene", scenePanelSize);
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
  Dockspace();
}

void Editor::RenderEditor(Scene* scene, FrameBuffer* framebuffer)
{
  if (_demoOpen) 
    ImGui::ShowDemoWindow(&_demoOpen);
  
  if (_preferencesOpen)
    ShowPreferences();
  
  if (_statsOpen)
    ShowStats();

  contentBrowserPanel->RenderPanel();

  viewportPanel->RenderPanel(framebuffer);

  scenePanel->RenderPanel(scene);
  
  if (_inspectorOpen)
    ShowInspector();

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

  //ImGuiStyle& style = ImGui::GetStyle();
  //style.FrameRounding = 6;

  /* When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones. */
  //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  //{
  //  style.WindowRounding = 0.0f;
  //  style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  //}

  ImGui::StyleColorsDark();
}

void Editor::Dockspace()
{
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResize;

  /* We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
   because it would be confusing to have two docking targets within each others. */
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  const ImGuiViewport* imViewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(imViewport->WorkPos);
  ImGui::SetNextWindowSize(imViewport->WorkSize);
  ImGui::SetNextWindowViewport(imViewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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

  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Hierarchy", nullptr, &scenePanel->isOpen);
      ImGui::MenuItem("Browser", nullptr, &contentBrowserPanel->isOpen);
      ImGui::MenuItem("Inspector", nullptr, &_inspectorOpen);
      ImGui::MenuItem("Viewport", nullptr, &viewportPanel->isOpen);
      ImGui::MenuItem("Statistics", nullptr, &_statsOpen);
      ImGui::MenuItem("Demo", nullptr, &_demoOpen);

      ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Preferences"))
    {
      _preferencesOpen = true;
    }

    ImGui::EndMenuBar();
  }
  ImGui::End();
}

void Editor::ShowStats()
{
  ImGuiIO& io = ImGui::GetIO();

  static float timeOld = 0.0f;
  static float timeNew = 0.0f;
  static float framerate = io.Framerate;

  ImGui::Begin("Stats", &_statsOpen);

  timeNew = glfwGetTime();
  if (timeNew - timeOld >= (1 / 8.0f)) /* update every 1/4 seconds */
  {
    timeOld = timeNew;
    framerate = io.Framerate;
  }

  ImGui::Text("Application average %.3f ms/frame (%d FPS)", 1000.0f / framerate, (int)framerate);
  ImGui::Text("Draw calls: %d", Renderer::drawCalls);
  ImGui::End();
}

void Editor::ShowInspector()
{
  ImGui::SetNextWindowSize(ImVec2(_inspectorSize.x, _inspectorSize.y));
  ImGui::Begin("Inspector", &_inspectorOpen);

  ImGui::End();
}

void Editor::ShowPreferences()
{
  auto& instanceCM = ConfigurationsManager::Instance();

  static bool buttonDisabled = true;
  ImGui::Begin("Preferences", &_preferencesOpen);
  ImGui::SeparatorText("Window properties");
  
  /* Window title */
  static String& title = instanceCM.GetValue(CONF_WINDOW_TITLE);
  if (ImGui::InputText("Title", &title))
    buttonDisabled = false;
  
  /* Window aspect ratio */
  if (ImGui::Combo("Aspect ratio", &_aspectIndex, _aspectRatioValues, 3)) /* 3 aspect ratio availables */
  {
    instanceCM.SetValue(CONF_ASPECT_RATIO, _aspectRatioValues[_aspectIndex]);
    buttonDisabled = false;
  }
  
  /* Window resolution */
  if (ImGui::Combo("Resolution", &_resolutionIndex, _resolutionValues, 12)) /* 12 resolution availables */
  {
    instanceCM.SetValue(CONF_WINDOW_RESOLUTION, _resolutionValues[_resolutionIndex]);
    buttonDisabled = false;
  }

  /* V-sync */
  static bool vsyncActive = (std::strcmp(instanceCM.GetValue(CONF_VSYNC).c_str(), "true") == 0);
  if (ImGui::Checkbox("V-sync", &vsyncActive))
  {
    instanceCM.SetValue(CONF_VSYNC, (vsyncActive ? "true" : "false"));
    buttonDisabled = false;
  }
  
  /* Disabling button */
  if (buttonDisabled)
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
  }
  /* Button event */
  bool save = ImGui::Button("Save");
  if (save)
    instanceCM.Save();
  /* Restore button */
  if (buttonDisabled)
  {
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
  }
  /* Tooltip */
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip())
  {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted("You have to restart the application to make the changes active");
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
  if(save)
    buttonDisabled = true;
  ImGui::End();
}
