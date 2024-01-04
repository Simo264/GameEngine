#include "Editor.hh"

#include "../Logger.hh"
#include "../Mesh/Model.hh"

#include "../Subsystems/FontsManager.hh"
#include "../Subsystems/TexturesManager.hh"
#include "../Graphics/Renderer.hh"

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
  // Init ImGui
  // ---------------------------------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

  Styling();

  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void Editor::ShutDown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Editor::NewFrame()
{
  //Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Editor::RenderFrame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void Editor::MenuBar()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("View"))
    {
      if (ImGui::MenuItem("Demo panel"))
        _demoPanelOpen = true;
      else if (ImGui::MenuItem("Scene panel"))
        _scenePanelOpen = true;
      else if (ImGui::MenuItem("Stats panel"))
        _statsPanelOpen = true;
      
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void Editor::ShowDemo()
{
  if (!_demoPanelOpen) return;

  ImGui::ShowDemoWindow(&_demoPanelOpen);
}

void Editor::ShowStats()
{
  if (!_statsPanelOpen)
    return;

  ImGui::Begin("Stats", &_statsPanelOpen);

  ImGuiIO& io = ImGui::GetIO();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::Text("Draw calls: %d", Renderer::drawCalls);
  ImGui::End();
}

void Editor::ShowScenePanel()
{
#if 0
  if (!_scenePanelOpen)
    return;

  static int treeNodeActorSelected = -1 ? sceneActors.empty() : 0;

  ImGui::Begin("Scene", &_scenePanelOpen);
  if (ImGui::TreeNode("Actors"))
  { 
    for (int i = 0; i < sceneActors.size(); i++)
    {
      if (ImGui::Selectable(sceneActors[i]->actorName.c_str(), treeNodeActorSelected == i))
        treeNodeActorSelected = i;
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Lighting"))
  {
    if (ImGui::TreeNode("Directional light"))
    {
      ImGui::ColorEdit3("Light color", (float*)&dirLight.color);
      ImGui::SliderFloat("Light ambient", &dirLight.ambient, 0.0f, 1.0f);
      ImGui::SliderFloat("Light diffuse", &dirLight.diffuse, 0.0f, 1.0f);
      ImGui::SliderFloat("Light specular", &dirLight.specular, 0.0f, 1.0f);
      ImGui::DragFloat3("Light direction", (float*)&dirLight.direction, 0.005f, -FLT_MAX, +FLT_MAX);
      ImGui::TreePop();
    }
    ImGui::TreePop();
  }
  ImGui::End();

  if (treeNodeActorSelected >= 0)
    ShowPropertiesPanel(sceneActors[treeNodeActorSelected]);
#endif
}

void Editor::ShowViewportPanel(const uint32_t& framebufferTexture)
{
#if 0
  ImGui::Begin("Viewport");

  // Using a Child allow to fill all the space of the window.
  // It also alows customization
  ImGui::BeginChild("GameRender");
  
  // Get the size of the child (i.e. the whole draw size of the windows).
  ImVec2 wsize = ImGui::GetWindowSize();

  // Because I use the texture from OpenGL, I need to invert the V from the UV.
  ImGui::Image((ImTextureID)framebufferTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));
  
  ImGui::EndChild();
  ImGui::End();
#endif
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Editor::Styling()
{
  auto fontPath = FontsManager::GetFont("Karla-Regular.ttf");
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); // custom font

  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 6;

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui::StyleColorsClassic();
}

void Editor::ShowPropertiesPanel()
{
#if 0
  ImGui::Begin("Properties");

  char text[50] = "Actor name: ";
  strcat_s(text, 50, target->actorName.c_str());
  ImGui::Text(text);

  ImGui::SeparatorText("Transformation");
  ImGui::DragFloat3("Translation", (float*)&target->position, 0.005f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat3("Scaling", (float*)&target->scaling, 0.005f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat("Rotation angle", (float*)&target->rotationAngle, 0.5f, -180.0f, +180.0f);
  ImGui::SliderFloat3("Rotation axis", (float*)&target->rotation, 0.0f, 1.0f);

  ImGui::SeparatorText("Material");
  ImGui::Text("Current texture");
  
  Model* modelTarget = static_cast<Model*>(target);
  Mesh& meshTarget = modelTarget->GetMesh(0);
  ImGui::Image(
    (ImTextureID)meshTarget.textureDiffuse->textureID,
    ImVec2(64.0f, 64.0f),
    ImVec2(0.0f, 0.0f),
    ImVec2(1.0f, 1.0f),
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
  );

  ImGui::SameLine();
  ImGui::Text(meshTarget.textureDiffuse->texturePath.filename().string().c_str());

  ImGui::End();
#endif
}
