#include "ImGuiLayer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Camera.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

extern void GUI_RenderMenuBar(Scene& scene);
extern void GUI_RenderHierarchy(Scene& scene, GameObject& objSelected);
extern void GUI_RenderViewport(u32 texID, GameObject& objSelected, i32 gizmode, const mat4f& view, const mat4f& proj);
extern void GUI_RenderInspector(GameObject& object);
extern void GUI_RenderTransformToolBar(vec2i viewportPos, i32& gizmode);
extern void Gui_RenderCameraSettings(Camera& camera);

// --------------------------
//          PUBLIC
// --------------------------

void ImGuiLayer::Initialize()
{
  // Setup ImGui context
  SetupContext();

  // Custom styling
  Styling();

  // Load default font
  g_fontFamily = "OpenSans/OpenSans-Regular.ttf";
  g_fontSize = 16;

  SetFont(Paths::GetFontsPath() / g_fontFamily, g_fontSize);
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
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  Docking();
}
void ImGuiLayer::EndFrame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    WindowManager& windowManager = WindowManager::Get();
    GLFWwindow* backupCurrentContext = windowManager.GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    windowManager.MakeContextCurrent(backupCurrentContext);
  }
}
void ImGuiLayer::SetFont(const fs::path& ttfFilePath, u32 fontSize) const
{
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontFromFileTTF(ttfFilePath.string().c_str(), static_cast<f32>(fontSize));
  io.Fonts->Build();

  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_CreateDeviceObjects();
}
void ImGuiLayer::RenderDemo()
{
  static bool open = true;
  if (open)
    ImGui::ShowDemoWindow(&open);
}
void ImGuiLayer::RenderMenuBar(Scene& scene) const
{
  GUI_RenderMenuBar(scene);
}
void ImGuiLayer::RenderViewport(u32 texture, GameObject& objSelected, const mat4f& view, const mat4f& proj) const
{
  GUI_RenderViewport(texture, objSelected, gizmode, view, proj);
}
GameObject& ImGuiLayer::RenderHierarchy(Scene& scene)
{
  static GameObject object;
  GUI_RenderHierarchy(scene, object);
  return object;
}
void ImGuiLayer::RenderInspector(GameObject& object)
{
  GUI_RenderInspector(object);
}
void ImGuiLayer::RenderGizmoToolBar()
{
  GUI_RenderTransformToolBar(viewportPos, gizmode);
}
void ImGuiLayer::RenderGraphicsInfo()
{
  ImGui::Begin("Graphics info");

  WindowManager& winManager = WindowManager::Get();
  static const char* glRender = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  static const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  static const char* glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  static const char* glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  ImGui::TextWrapped("GLFW: %s\nOpenGL renderer: %s\nOpenGL version: %s\nOpenGL vendor: %s\nOpenGL Shading Language Version: %s",
    winManager.GetVersion(),
    glRender,
    glVersion,
    glVendor,
    glsl);

  ImGui::End();
}
void ImGuiLayer::RenderTimeInfo(f64 delta, f64 avg, i32 frameRate)
{
  constexpr i32 flags = ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBackground;

  constexpr ImVec2 windowSize{ 160.f, 100.f };
  ImVec2 windowPos = ImVec2(
    static_cast<f32>((viewportPos.x + viewportSize.x) - windowSize.x),
    static_cast<f32>(viewportPos.y + 25)
  );
  ImGui::SetNextWindowPos(windowPos);
  ImGui::SetNextWindowSize(windowSize);
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("Info", nullptr, flags);

  ImGui::TextWrapped("Time (ms): %f", delta * 1000.0f);
  ImGui::TextWrapped("Average (ms): %f", avg * 1000.0f);
  ImGui::TextWrapped("Frame rate: %d", frameRate);

  ImGui::End();
}
void ImGuiLayer::RenderCameraSettings(Camera& camera)
{
  Gui_RenderCameraSettings(camera);
}

// --------------------------
//          PRIVATE
// --------------------------

void ImGuiLayer::SetupContext()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   /* Enable Multi-Viewport / Platform Windows */

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
  colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
  colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Buttons
  colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
  colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

  // Popups
  colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

  // Slider
  colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
  colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

  // Frame BG
  colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
  colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
  colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

  // Tabs
  colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
  colors[ImGuiCol_TabHovered] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
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
  constexpr ImGuiWindowFlags windowFlags =
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

  static bool firstTime = true;
  if (firstTime)
  {
    firstTime = false;

    ImGui::DockBuilderRemoveNode(dockspaceID);
    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->WorkSize);

    ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.25f, nullptr, &dockspaceID);
    ImGuiID dockMain = dockspaceID;
    
    ImGuiID dockLeftTop;
    ImGuiID dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.5f, nullptr, &dockLeftTop);

    ImGui::DockBuilderDockWindow("Hierarchy", dockLeftTop);
    ImGui::DockBuilderDockWindow("Camera settings", dockLeftTop);
    ImGui::DockBuilderDockWindow("Inspector", dockLeftBottom);
    ImGui::DockBuilderDockWindow("Viewport", dockMain);

    ImGui::DockBuilderFinish(dockspaceID);
  }

  ImGui::End();
}
