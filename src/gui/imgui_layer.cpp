#include "imgui_layer.hpp"

#include "engine/globals.hpp"
#include "engine/paths.hpp"
#include "engine/ecs/scene.hpp"
#include "engine/graphics/texture2d.hpp"
#include "engine/managers/window_manager.hpp"

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

void ImGuiLayer::initialize_imgui(Scene& scene)
{
  render_imgui_demo = false;
  render_time_info = true;
  render_graphics_info = false;

  // Setup ImGui context
  setup_imgui_context();

  // Custom styling
  customize_style();

  // Load font
  auto& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontFromFileTTF((GetFontsPath() / FONT_FAMILY).string().c_str(), FONT_SIZE);
  io.Fonts->Build();
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_CreateDeviceObjects();

  viewport.set_scene(&scene);
  toolbar.set_scene(&scene);
  hierarchy.set_scene(&scene);
  inspector.set_scene(&scene);
}
void ImGuiLayer::cleanup_imgui()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
void ImGuiLayer::prepare_imgui_frame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  configure_dockspace();
}
void ImGuiLayer::complete_frame_render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  auto& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    auto& window_manager = WindowManager::GetInstance();
    auto current_context = window_manager.GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    window_manager.MakeContextCurrent(current_context);
  }
}

void ImGuiLayer::imgui_demo()
{
  if (render_imgui_demo)
    ImGui::ShowDemoWindow(&render_imgui_demo);
}
void ImGuiLayer::DebugInfo(f64 delta, f64 avg, i32 frameRate)
{
  if (!render_time_info)
    return;

  constexpr i32 flags = ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBackground;

  constexpr auto window_size = ImVec2{ 160.f, 100.f };
  auto window_pos = ImVec2(static_cast<f32>((viewport.position.x + viewport.width) - window_size.x),
                          static_cast<f32>(viewport.position.y + 25));
  ImGui::SetNextWindowPos(window_pos);
  ImGui::SetNextWindowSize(window_size);
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("Info", &render_time_info, flags);
  ImGui::TextWrapped("Time (ms): %f", delta * 1000.0f);
  ImGui::TextWrapped("Average (ms): %f", avg * 1000.0f);
  ImGui::TextWrapped("Frame rate: %d", frame_rate);
  ImGui::TextWrapped("Draw calls: %d", g_draw_calls);
  ImGui::End();
}
void ImGuiLayer::graphics_info()
{
  if (!render_graphics_info)
    return;

  constexpr auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
  ImGui::SetNextWindowSize(ImVec2(468, 128), ImGuiCond_Once);
  ImGui::Begin("Graphics info", &render_graphics_info, flags);

  auto& win_manager = WindowManager::instance();
  static auto gl_render = glGetString(GL_RENDERER);
  static auto gl_version = glGetString(GL_VERSION);
  static auto gl_vendor = glGetString(GL_VENDOR);
  static auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ImGui::TextWrapped("GLFW: %s\nOpenGL renderer: %s\nOpenGL version: %s\nOpenGL vendor: %s\nOpenGL Shading Language Version: %s",
                     win_manager.GetVersion(),
                     gl_render,
                     gl_version,
                     gl_vendor,
                     glsl);
  ImGui::End();
}

// --------------------------
//          PRIVATE
// --------------------------

void ImGuiLayer::setup_imgui_context()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; /* Enable Keyboard Controls */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  /* Enable Gamepad Controls */
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     /* Enable Docking */
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   /* Enable Multi-Viewport / Platform Windows */

  ImGui_ImplGlfw_InitForOpenGL(WindowManager::instance().GetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 460");
}
void ImGuiLayer::customize_style()
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
void ImGuiLayer::configure_dockspace()
{
  constexpr auto window_flags = 
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

  ImGui::Begin("Dockspace", nullptr, window_flags);
  ImGui::PopStyleVar(3);

  auto dockspace_id = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

  static auto first_time = true;
  if (first_time)
  {
    first_time = false;

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

    auto dockLeft = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
    auto dockMain = dockspace_id;
    auto dockLeftTop = ImGuiID{};
    auto dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.5f, nullptr, &dockLeftTop);
    auto dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain);

    ImGui::DockBuilderDockWindow("Hierarchy", dockLeftTop);
    ImGui::DockBuilderDockWindow("Camera Properties", dockLeftTop);
    ImGui::DockBuilderDockWindow("Inspector", dockLeftBottom);
    ImGui::DockBuilderDockWindow("Viewport", dockMain);
    ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockRight);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::End();
}
