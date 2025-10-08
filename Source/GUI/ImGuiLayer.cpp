#include "ImGuiLayer.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Paths.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Managers/WindowManager.hpp"

#include <glad/gl.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

constexpr auto FONT_SIZE = 16.f;
constexpr auto FONT_FAMILY = "OpenSans/OpenSans-Regular.ttf";

// --------------------------
//          PUBLIC
// --------------------------

void ImGuiLayer::InitializeImGui(Scene& scene)
{
  renderImGuiDemo = false;
  renderTimeInfo = true;
  renderGraphicsInfo = false;

  // Setup ImGui context
  __SetupImGuiContext();

  // Custom styling
  __CustomizeStyle();

  // Load font
  auto& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontFromFileTTF((GetFontsPath() / FONT_FAMILY).string().c_str(), FONT_SIZE);
  io.Fonts->Build();
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_CreateDeviceObjects();

  viewport.SetScene(&scene);
  toolbar.SetScene(&scene);
  hierarchy.SetScene(&scene);
  inspector.SetScene(&scene);
}
void ImGuiLayer::CleanupImGui()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
void ImGuiLayer::PrepareImGuiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  __ConfigureDockspace();
}
void ImGuiLayer::CompleteFrameRender()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  auto& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    auto& windowManager = WindowManager::GetInstance();
    auto backupCurrentContext = windowManager.GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    windowManager.MakeContextCurrent(backupCurrentContext);
  }
}

void ImGuiLayer::ImguiDemo()
{
  if (renderImGuiDemo)
    ImGui::ShowDemoWindow(&renderImGuiDemo);
}
void ImGuiLayer::DebugInfo(f64 delta, f64 avg, i32 frameRate)
{
  if (!renderTimeInfo)
    return;

  constexpr i32 flags = ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBackground;

  constexpr auto windowSize = ImVec2{ 160.f, 100.f };
  auto windowPos = ImVec2(static_cast<f32>((viewport.position.x + viewport.size.x) - windowSize.x),
                          static_cast<f32>(viewport.position.y + 25));
  ImGui::SetNextWindowPos(windowPos);
  ImGui::SetNextWindowSize(windowSize);
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("Info", &renderTimeInfo, flags);
  ImGui::TextWrapped("Time (ms): %f", delta * 1000.0f);
  ImGui::TextWrapped("Average (ms): %f", avg * 1000.0f);
  ImGui::TextWrapped("Frame rate: %d", frameRate);
  ImGui::TextWrapped("Draw calls: %d", g_DrawCalls);
  ImGui::End();
}
void ImGuiLayer::GraphicsInfo()
{
  if (!renderGraphicsInfo)
    return;

  constexpr auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
  ImGui::SetNextWindowSize(ImVec2(468, 128), ImGuiCond_Once);
  ImGui::Begin("Graphics info", &renderGraphicsInfo, flags);

  auto& winManager = WindowManager::GetInstance();
  static auto glRender = glGetString(GL_RENDERER);
  static auto glVersion = glGetString(GL_VERSION);
  static auto glVendor = glGetString(GL_VENDOR);
  static auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ImGui::TextWrapped("GLFW: %s\nOpenGL renderer: %s\nOpenGL version: %s\nOpenGL vendor: %s\nOpenGL Shading Language Version: %s",
                     winManager.GetVersion(),
                     glRender,
                     glVersion,
                     glVendor,
                     glsl);
  ImGui::End();
}

// --------------------------
//          PRIVATE
// --------------------------

void ImGuiLayer::__SetupImGuiContext()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   /* Enable Multi-Viewport / Platform Windows */

  ImGui_ImplGlfw_InitForOpenGL(WindowManager::GetInstance().GetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
}
void ImGuiLayer::__CustomizeStyle()
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
void ImGuiLayer::__ConfigureDockspace()
{
  constexpr auto windowFlags = 
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus |
    ImGuiWindowFlags_NoMove;

  auto viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  ImGui::Begin("Dockspace", nullptr, windowFlags);
  ImGui::PopStyleVar(3);

  auto dockspaceID = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

  static auto firstTime = true;
  if (firstTime)
  {
    firstTime = false;

    ImGui::DockBuilderRemoveNode(dockspaceID);
    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->WorkSize);

    auto dockLeft = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.25f, nullptr, &dockspaceID);
    auto dockMain = dockspaceID;
    auto dockLeftTop = ImGuiID{};
    auto dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.5f, nullptr, &dockLeftTop);
    auto dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain);

    ImGui::DockBuilderDockWindow("Hierarchy", dockLeftTop);
    ImGui::DockBuilderDockWindow("Camera Properties", dockLeftTop);
    ImGui::DockBuilderDockWindow("Inspector", dockLeftBottom);
    ImGui::DockBuilderDockWindow("Viewport", dockMain);
    ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockRight);

    ImGui::DockBuilderFinish(dockspaceID);
  }

  ImGui::End();
}
