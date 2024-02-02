#include "Editor.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Mesh/StaticMesh.hh"
#include "../FrameBuffer.hh"
#include "../Scene.hh"

#include "../Subsystems/TexturesManager.hh"
#include "../Subsystems/FontsManager.hh"
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

  String& resolutionStr = ConfigurationsManager::GetValue(CONF_WINDOW_RESOLUTION);
  String& aspectRatioStr = ConfigurationsManager::GetValue(CONF_ASPECT_RATIO);
  for (aspectIndex = 0; aspectIndex < 3; aspectIndex++)
    if (std::strcmp(aspectRatioValues[aspectIndex], aspectRatioStr.c_str()) == 0)
      break;

  for (resolutionIndex = 0; resolutionIndex < 12; resolutionIndex++)
    if (std::strcmp(resolutionValues[resolutionIndex], resolutionStr.c_str()) == 0)
      break;

  Vec2i resolution = ConfigurationsManager::ParseResolution(resolutionStr);

  _viewportSize  = Vec2i(resolution.x * 0.6, resolution.y * 0.6); /* 60% x 60%  */
  _hierarchySize = Vec2i(resolution.x * 0.2, resolution.y * 1);   /* 20% x 100% */
  _inspectorSize = Vec2i(resolution.x * 0.2, resolution.y * 1);   /* 20% x 100% */
  _browserSize   = Vec2i(resolution.x * 0.6, resolution.y * 0.4); /* 60% x 40%  */
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

void Editor::RenderFrame(Scene* scene, FrameBuffer* framebuffer)
{
  if (_demoOpen) 
    ImGui::ShowDemoWindow(&_demoOpen);
  
  if (_preferencesOpen)
    ShowPreferences();
  
  if (_statsOpen)
    ShowStats();
  
  if (_hierarchyOpen)
    ShowHierarchy(scene);
  
  if (_viewportOpen)
    ShowViewport(framebuffer);
  
  if (_browserOpen)
    ShowBrowser();
  
  if (_inspectorOpen)
    ShowInspector();
  
  ImGui::Render();
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
  auto fontPath = FontsManager::GetFont("Karla-Regular.ttf");
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); // custom font

  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 6;

  /* When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones. */
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui::StyleColorsClassic();
}

void Editor::Dockspace()
{
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoResize;

  /* We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
   because it would be confusing to have two docking targets within each others. */
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
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
      ImGui::MenuItem("Hierarchy", nullptr, &_hierarchyOpen);
      ImGui::MenuItem("Browser", nullptr, &_browserOpen);
      ImGui::MenuItem("Inspector", nullptr, &_inspectorOpen);
      ImGui::MenuItem("Viewport", nullptr, &_viewportOpen);
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

void Editor::ShowHierarchy(Scene* scene)
{
  auto& dirLight = scene->directionalLight;
  auto& pointLights = scene->pointLights;
  auto& sceneMeshes = scene->statMeshes;

  static bool dirLightSelected = false;
  static int pointLightSelected = -1;
  static int staticMeshSelected = -1;
  
  char digits[8]{};
  static String treeName(32, 0);
  static String treeNode(32, 0);

  ImGui::SetNextWindowSize(ImVec2(_hierarchySize.x, _hierarchySize.y));
  ImGui::Begin("Hierarchy", &_hierarchyOpen);

  /* Lighting Tree */
  if (ImGui::TreeNode("Lighting"))
  {
    /* Directional light node */
    if (ImGui::Selectable("Directional light", dirLightSelected == true))
    {
      _propertiesOpen = true;
      dirLightSelected = true;
      staticMeshSelected = -1;
      pointLightSelected = -1;
    }

    /* Point light nodes */
    if (pointLights.size() > 0)
    {
      _itoa_s(pointLights.size(), digits, 10);
      treeName.clear();
      treeName.append("Point lights");
      treeName.append(" (");
      treeName.append(digits);
      treeName.append(")");
      if (ImGui::TreeNode(treeName.c_str()))
      {
        for (int i = 0; i < pointLights.size(); i++)
        {
          _itoa_s(i, digits, 10);
          treeNode.clear();
          treeNode.append("Light");
          treeNode.append(digits);
          if (ImGui::Selectable(treeNode.c_str(), pointLightSelected == i))
          {
            _propertiesOpen = true;
            dirLightSelected = false;
            pointLightSelected = i;
            staticMeshSelected = -1;
          }
        }
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }


  /* Static mesh Tree */
  _itoa_s(sceneMeshes.size(), digits, 10);
  treeName.clear();
  treeName.append("StaticMesh");
  treeName.append(" (");
  treeName.append(digits);
  treeName.append(")");
  if (ImGui::TreeNode(treeName.c_str()))
  {
    /* Static mesh nodes */
    for (int i = 0; i < sceneMeshes.size(); i++)
    {
      _itoa_s(i, digits, 10);
      treeNode.clear();
      treeNode.append("StaticMesh");
      treeNode.append(digits);
      if (ImGui::Selectable(treeNode.c_str(), staticMeshSelected == i))
      {
        _propertiesOpen = true;
        dirLightSelected = false;
        pointLightSelected = -1;
        staticMeshSelected = i;
      }
    }
    ImGui::TreePop();
  }
  ImGui::End();

  if (_propertiesOpen && dirLightSelected)
    ShowPropertiesPanel(dirLight);
  else if (_propertiesOpen && pointLightSelected >= 0)
    ShowPropertiesPanel(pointLights[pointLightSelected]);
  else if (_propertiesOpen && staticMeshSelected >= 0)
    ShowPropertiesPanel(sceneMeshes[staticMeshSelected]);
}

void Editor::ShowViewport(FrameBuffer* framebuffer)
{
  ImGui::SetNextWindowSize(ImVec2(_viewportSize.x, _viewportSize.y));
  ImGui::Begin("Viewport", &_viewportOpen);

  _isViewportFocused = ImGui::IsWindowFocused();

  /* Using a Child allow to fill all the space of the window. It also alows customization */
  ImGui::BeginChild("GameRender");

  _isViewportFocused = _isViewportFocused || ImGui::IsWindowFocused();
 
  /* Get the size of the child(i.e.the whole draw size of the windows). */
  ImVec2 drawSpace = ImGui::GetWindowSize();
  Vec2i framebufferSize = framebuffer->GetSize();
  if((drawSpace.x != framebufferSize.x || drawSpace.y != framebufferSize.y))
  {
    framebuffer->RescaleFrameBuffer({ drawSpace.x, drawSpace.y});
    glViewport(0, 0, drawSpace.x, drawSpace.y);
    _viewportSize.x = drawSpace.x;
    _viewportSize.y = drawSpace.y;
  }
  else
  {
    /* Because I use the texture from OpenGL, I need to invert the V from the UV. */
    ImGui::Image((ImTextureID)framebuffer->GetImage(), drawSpace, ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::EndChild();
  ImGui::End();
}

void Editor::ShowBrowser()
{
  ImGui::SetNextWindowSize(ImVec2(_browserSize.x, _browserSize.y));
  ImGui::Begin("Browser", &_browserOpen);
  
  ImGui::End();
}

void Editor::ShowInspector()
{
  ImGui::SetNextWindowSize(ImVec2(_inspectorSize.x, _inspectorSize.y));
  ImGui::Begin("Inspector", &_inspectorOpen);

  ImGui::End();
}

void Editor::ShowPropertiesPanel(StaticMesh* meshTarget)
{
  static Vector<Texture2D*> textures;
  if (textures.empty())
    TexturesManager::GetTextures(textures);

  ImGui::Begin("Properties", &_propertiesOpen);
  ImGui::Text("(StaticMesh name)");
  
  ImGui::SeparatorText("Transformation");
  ImGui::DragFloat3("Translation", (float*)&meshTarget->position, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat3("Scaling", (float*)&meshTarget->scaling, 0.1f, -FLT_MAX, +FLT_MAX);
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
    ImVec2(64.0f, 64.0f) /* image size */
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
  ImGui::Begin("Properties", &_propertiesOpen);
  ImGui::SeparatorText("Directional light");

  ImGui::ColorEdit3("Color", (float*)&dirLight->color);
  ImGui::SliderFloat("Ambient", &dirLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &dirLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &dirLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (float*)&dirLight->direction, 0.1f, -FLT_MAX, +FLT_MAX);
  
  ImGui::End();
}

void Editor::ShowPropertiesPanel(PointLight* pointLight)
{
  ImGui::Begin("Properties", &_propertiesOpen);
  ImGui::SeparatorText("Point light");
  
  ImGui::ColorEdit3("Color", (float*)&pointLight->color);
  ImGui::SliderFloat("Ambient", &pointLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &pointLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &pointLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (float*)&pointLight->position, 0.1f, -FLT_MAX, +FLT_MAX);
  
  ImGui::End();
}

void Editor::ShowPreferences()
{
  static bool buttonDisabled = true;
  ImGui::Begin("Preferences", &_preferencesOpen);
  ImGui::SeparatorText("Window properties");
  
  /* Window title */
  static String& title = ConfigurationsManager::GetValue(CONF_WINDOW_TITLE);
  if (ImGui::InputText("Title", &title))
    buttonDisabled = false;
  
  /* Window aspect ratio */
  if (ImGui::Combo("Aspect ratio", &aspectIndex, aspectRatioValues, 3)) /* 3 aspect ratio availables */
  {
    ConfigurationsManager::SetValue(CONF_ASPECT_RATIO, aspectRatioValues[aspectIndex]);
    buttonDisabled = false;
  }
  
  /* Window resolution */
  if (ImGui::Combo("Resolution", &resolutionIndex, resolutionValues, 12)) /* 12 resolution availables */
  {
    ConfigurationsManager::SetValue(CONF_WINDOW_RESOLUTION, resolutionValues[resolutionIndex]);
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
    ConfigurationsManager::Save();
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
