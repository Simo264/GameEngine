#include "Editor.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Scene.hh"

#include "../Subsystems/TexturesManager.hh"
#include "../Subsystems/FontsManager.hh"
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

  ImGuiIO& io = ImGui::GetIO();

  static float timeOld = 0.0f;
  static float timeNew = 0.0f;
  static float framerate = io.Framerate;

  ImGui::Begin("Stats", &_statsPanelOpen);

  timeNew = glfwGetTime();
  if (timeNew - timeOld >= (1/8.0f)) // update every 1/4 seconds
  {
    timeOld = timeNew;
    framerate = io.Framerate;
  }
  
  ImGui::Text("Application average %.3f ms/frame (%d FPS)", 1000.0f / framerate, (int) framerate);
  ImGui::Text("Draw calls: %d", Renderer::drawCalls);
  ImGui::End();
}

void Editor::ShowScenePanel(Scene* scene)
{
  if (!_scenePanelOpen)
    return;
  
  auto& dirLight = scene->directionalLight;
  auto& pointLights = scene->pointLights;
  auto& sceneMeshes = scene->statMeshes;

  static bool treeNodeDirLightSelected = false;
  static int treeNodePointLightSelected = -1; 
  static int treeNodeStaticMeshSelected = -1;

  ImGui::Begin("Scene", &_scenePanelOpen);

  if (ImGui::TreeNode("Lighting"))
  {
    if (ImGui::Selectable("Directional light", treeNodeDirLightSelected == true))
    {
      treeNodeDirLightSelected = true;
      treeNodeStaticMeshSelected = -1;
      treeNodePointLightSelected = -1;
    }

    if (pointLights.size() > 0)
    {
      if (ImGui::TreeNode("Point lights"))
      {
        for (int i = 0; i < pointLights.size(); i++)
        {
          char pointLightName[50] = "Point light";
          std::strcat(pointLightName, std::to_string(i).c_str());
          if (ImGui::Selectable(pointLightName, treeNodePointLightSelected == i))
          {
            treeNodeDirLightSelected = false;
            treeNodePointLightSelected = i;
            treeNodeStaticMeshSelected = -1;
          }
        }
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }


  if (ImGui::TreeNode("StaticMesh"))
  {
    for (int i = 0; i < sceneMeshes.size(); i++)
    {
      char staticMeshName[30] = "StaticMesh";
      std::strcat(staticMeshName, std::to_string(i).c_str());

      if (ImGui::Selectable(staticMeshName, treeNodeStaticMeshSelected == i))
      {
        treeNodeDirLightSelected = false;
        treeNodePointLightSelected = -1;
        treeNodeStaticMeshSelected = i;
      }
    }
    ImGui::TreePop();
  }
  ImGui::End();

  if (treeNodeDirLightSelected) 
    ShowPropertiesPanel(dirLight);
  else if (treeNodePointLightSelected >= 0) 
    ShowPropertiesPanel(pointLights[treeNodePointLightSelected]);
  else if (treeNodeStaticMeshSelected >= 0)
    ShowPropertiesPanel(sceneMeshes[treeNodeStaticMeshSelected]);
}

#if 0
void Editor::ShowViewportPanel(const uint32_t& framebufferTexture)
{
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
}
#endif


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

void Editor::ShowPropertiesPanel(StaticMesh* meshTarget)
{
  static Vector<Texture2D*> textures;
  if (textures.empty())
    TexturesManager::GetTextures(textures);

  ImGui::Begin("Properties");
  ImGui::Text("(StaticMesh name)");
  
  ImGui::SeparatorText("Transformation");
  ImGui::DragFloat3("Translation", (float*)&meshTarget->position, 0.005f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat3("Scaling", (float*)&meshTarget->scaling, 0.005f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat("Rotation angle", (float*)&meshTarget->rotationAngle, 0.5f, -180.0f, +180.0f);
  ImGui::SliderFloat3("Rotation axis", (float*)&meshTarget->rotationAxis, 0.0f, 1.0f);

  ImGui::SeparatorText("Material");

  const String diffusePathStr = (meshTarget->diffuse ? meshTarget->diffuse->texturePath.string() : "");
  const uint32_t diffuseID = (meshTarget->diffuse ? meshTarget->diffuse->textureID : 0);
  if (meshTarget->diffuse)
    ImGui::Text(diffusePathStr.c_str());
  else
    ImGui::Text("None");
  
  ImGui::Image(
    (ImTextureID)diffuseID,
    ImVec2(64.0f, 64.0f) // image size
  );

  if (ImGui::BeginCombo("Textures", diffusePathStr.c_str()))
  {
    for (int i = 0; i < textures.size(); i++)
    {
      String textPathStr = textures[i]->texturePath.string();
      bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
      if (ImGui::Selectable(textPathStr.c_str(), isSelected))
        meshTarget->diffuse = TexturesManager::GetTexture(textPathStr.c_str());
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::End();
}

void Editor::ShowPropertiesPanel(DirectionalLight* dirLight)
{
  ImGui::Begin("Properties");
  ImGui::SeparatorText("Directional light");

  ImGui::ColorEdit3("Light color", (float*)&dirLight->color);
  ImGui::SliderFloat("Light ambient", &dirLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Light diffuse", &dirLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Light specular", &dirLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Light direction", (float*)&dirLight->direction, 0.005f, -FLT_MAX, +FLT_MAX);
  
  ImGui::End();
}

void Editor::ShowPropertiesPanel(PointLight* pointLight)
{
  ImGui::Begin("Properties");
  ImGui::SeparatorText("Point light");
  
  ImGui::ColorEdit3("Light color", (float*)&pointLight->color);
  ImGui::SliderFloat("Light ambient", &pointLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Light diffuse", &pointLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Light specular", &pointLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Light position", (float*)&pointLight->position, 0.005f, -FLT_MAX, +FLT_MAX);
  
  ImGui::End();
}