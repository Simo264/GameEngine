#include "ImGuiLayer.hpp"

#include "Core/GL.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/FontManager.hpp"
#include "Engine/Filesystem/ConfigFile.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

extern void GUI_RenderMenuBar(Scene& scene, bool& openPreferences);
extern void GUI_RenderPreferencesFrame(bool& open, i32 fontSize);
extern void GUI_RenderOutliner(bool& open, Scene& scene, GameObject& objSelected);
extern void GUI_RenderViewport(bool& open, u32 texID, GameObject& objSelected, i32 gizmode, const mat4f& view, const mat4f& proj);
extern void GUI_RenderObjectDetails(bool& open, GameObject& object, i32& gizmode);
extern void GUI_RenderContentBrowser(bool& open);
extern void GUI_RenderWorldProperties(bool& open);
extern void GUI_RenderCameraProperties(bool& open, Camera& camera);

static i32 fontSize;
static i32 gizmode = ImGuizmo::OPERATION::TRANSLATE;

/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

void ImGuiLayer::Initialize()
{
  /* Setup ImGui context */
  SetupContext();
  
  /* Custom styling */
  Styling();

  /* Load default font */
  ConfigFile config((GetRootPath() / "Configuration.ini").string());
  config.ReadData();
  const String& fontFamily = config.GetValue("GUI", "font-family");
  fontSize = std::atoi(config.GetValue("GUI", "font-size").c_str());
  auto record = FontManager::Get().GetRecordByName(fontFamily.c_str());
  selectedFont = std::make_pair(const_cast<String*>(&record->first), const_cast<fs::path*>(&record->second));
  SetFont(selectedFont.second->string());
}
void ImGuiLayer::CleanUp()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
void ImGuiLayer::BeginFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::Begin();
  ImGuizmo::BeginFrame();

  Docking();
}
void ImGuiLayer::EndFrame()
{
  ImGui::RenderPanel();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    WindowManager& windowManager = WindowManager::Get();
    WindowManager::Context backCurrentContext = windowManager.GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    windowManager.MakeContextCurrent(backCurrentContext);
  }
}
void ImGuiLayer::SetFont(StringView fontPath)
{
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontFromFileTTF(fontPath.data(), fontSize);
  io.Fonts->Build();
  
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_CreateDeviceObjects();
}
void ImGuiLayer::Demo()
{
  static bool open = true;
  if(open)
    ImGui::ShowDemoWindow(&open);
}
void ImGuiLayer::MenuBar(Scene& scene)
{
  static bool viewPrefWindow = false;
  GUI_RenderMenuBar(scene, viewPrefWindow);

  /* Render preferences window */
  if (viewPrefWindow)
    GUI_RenderPreferencesFrame(viewPrefWindow, fontSize);
}
void ImGuiLayer::Viewport(u32 textureID, GameObject& objSelected, const mat4f& view, const mat4f& proj)
{
  static bool open = true;
  if (open)
    GUI_RenderViewport(open, textureID, objSelected, gizmode, view, proj);
}
GameObject& ImGuiLayer::OutlinerPanel(Scene& scene)
{
  static bool open = true;
  static GameObject object;
  
  if(open)
    GUI_RenderOutliner(open, scene, object);
  
  return object;
}
void ImGuiLayer::GameObjectDetails(GameObject& object)
{
  static bool open = true;
  if (open)
    GUI_RenderObjectDetails(open, object, gizmode);
}
void ImGuiLayer::ContentBrowser()
{
  static bool open = true;
  if (open)
    GUI_RenderContentBrowser(open);
}
void ImGuiLayer::WorldProps()
{
  static bool open = true;
  if (open)
    GUI_RenderWorldProperties(open);
}
void ImGuiLayer::CameraProps(Camera& camera)
{
  static bool open = true;
  if (open)
    GUI_RenderCameraProperties(open, camera);
}
void ImGuiLayer::DebugInfo(f64 delta, f64 avg, i32 frameRate, bool shadowMode, bool normalMode, bool wireframeMode)
{
  static constexpr const ImGuiWindowFlags flags =
    ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;

  ImGui::SetNextWindowPos(ImVec2(viewportPos.x, viewportPos.y + 20));
  ImGui::SetNextWindowSize(ImVec2(300.0f, 250.0f));
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("Info", nullptr, flags);

  auto& winManager = WindowManager::Get();
  static const char* glRender = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  static const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  static const char* glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  static const char* glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  ImGui::TextWrapped("GLFW: %s\nOpenGL renderer: %s\nOpenGL version: %s\nOpenGL vendor: %s\nOpenGL Shading Language Version: %s",
    winManager.GetVersion(),
    glRender,
    glVersion,
    glVendor,
    glsl
  );

  ImGui::Separator();

  ImGui::TextWrapped("Time (ms): %f", delta * 1000.0f);
  ImGui::TextWrapped("Average (ms): %f", avg * 1000.0f);
  ImGui::TextWrapped("Frame rate: %d", frameRate);

  ImGui::Separator();
  ImGui::TextWrapped("Shadow mode (F1 on/F2 off): %d", shadowMode);
  ImGui::TextWrapped("Normal mapping (F5 on/F6 off): %d", normalMode);
  ImGui::TextWrapped("Wireframe mode (F9 on/F10 off): %d", wireframeMode);

  ImGui::End();
}

void ImGuiLayer::DebugDepthMap(u32 tetxureID)
{
  ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 paddingTmp = style.WindowPadding;
  style.WindowPadding = { 0.0f, 0.0f };

  ImGui::Begin("Depth map", nullptr);
  ImGui::BeginChild("Map");

  ImGui::Image(reinterpret_cast<void*>(tetxureID), { 1024,1024 }, ImVec2(0, 1), ImVec2(1, 0));

  ImGui::EndChild();
  ImGui::End();

  style.WindowPadding = paddingTmp;
}
void ImGuiLayer::Test()
{
  ImGui::Begin("Test", nullptr);

  ImGui::End();
}

/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */

ImGuiLayer::ImGuiLayer()
  : viewportFocused{ false },
    viewportSize{},
    viewportPos{},
    selectedFont{},
    changeFontFamily{ false }
{}
void ImGuiLayer::SetupContext()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       /* Enable Multi-Viewport / Platform Windows */

  ImGui_ImplGlfw_InitForOpenGL(WindowManager::Get().GetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
}
void ImGuiLayer::Styling()
{
  auto& colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
  colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Border
  colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
  colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

  // Text
  colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
  colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

  // Headers
  colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
  colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Buttons
  colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
  colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

  // Popups
  colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

  // Slider
  colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
  colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

  // Frame BG
  colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
  colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Tabs
  colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
  colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
  colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Title
  colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Scrollbar
  colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
  colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

  // Seperator
  colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
  colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
  colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

  // Resize Grip
  colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
  colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
  colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

  // Docking
  colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

  auto& style = ImGui::GetStyle();
  style.TabRounding = 4;
  style.ScrollbarRounding = 9;
  style.WindowRounding = 7;
  style.GrabRounding = 3;
  style.FrameRounding = 3;
  style.PopupRounding = 4;
  style.ChildRounding = 4;
}
void ImGuiLayer::Docking()
{
  ImGuiWindowFlags windowFlags = 
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus |
    ImGuiWindowFlags_NoMove;

  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  ImGui::Begin("Dockspace", nullptr, windowFlags);

  ImGui::PopStyleVar(3);
  ImGuiID dockspaceID = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();
}
