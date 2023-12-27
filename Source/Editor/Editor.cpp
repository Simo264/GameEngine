#include "Editor.hh"
#include "../Logger.hh"

#include "../ResourceManager/FontsManager.hh"

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

void Editor::ShowDemo()
{
  ImGui::ShowDemoWindow(nullptr);
}

void Editor::ShowHelloWorld()
{
  ImGui::Begin("Hello world");
  ImGui::Text("This is some useful text.");              
  ImGui::End();
}

void Editor::ShowScenePanel(vector<Actor*>& sceneActors, DirectionalLight& dirLight)
{
  ImGui::Begin("Scene");
  
  if (ImGui::TreeNode("Actors"))
  { 
    for (Actor* actor : sceneActors)
    {
      if (ImGui::TreeNode(actor->actorName.c_str()))
      {
        //ImGui::SeparatorText("Inputs");
        //ImGui::InputText("Actor name", &actor->actorName, 50);

        ImGui::SeparatorText("Transformations");
        ImGui::DragFloat3("Translation", (float*)&actor->position, 0.005f, -FLT_MAX, +FLT_MAX);
        ImGui::DragFloat3("Scaling", (float*)&actor->scaling, 0.005f, -FLT_MAX, +FLT_MAX);
        
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Lighting"))
  {
    ImGui::SeparatorText("Directional light");
    ImGui::ColorEdit3("Light color", (float*)&dirLight.color);
    ImGui::SliderFloat("Light ambient", &dirLight.ambient, 0.0f, 1.0f);
    ImGui::SliderFloat("Light diffuse", &dirLight.diffuse, 0.0f, 1.0f);
    ImGui::SliderFloat("Light specular", &dirLight.specular, 0.0f, 1.0f);
    ImGui::DragFloat3("Light direction", (float*)&dirLight.direction, 0.005f, -FLT_MAX, +FLT_MAX);

    ImGui::TreePop();
  }

  ImGui::End();
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