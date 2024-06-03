#include "ImGuiLayer.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

#include <GLFW/glfw3.h>

namespace ImGuiLayer
{
  void SetupContext() 
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     /* Enable Keyboard Controls */
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      /* Enable Gamepad Controls */
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         /* Enable Docking */
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       /* Enable Multi-Viewport / Platform Windows */
    
    ImGui_ImplGlfw_InitForOpenGL(WindowManager::Instance()->GetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
  }
  void CleanUp()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void SetFont(const fspath& fontpath, int fontsize)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(fontpath.string().c_str(), fontsize);
  }

  void NewFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::Begin();
  }
  void DrawData()
  {
    auto window = WindowManager::Instance();

    ImGui::RenderPanel();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      WindowManager::Context backCurrentContext = window->GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      window->MakeContextCurrent(backCurrentContext);
    }
  }

  void Docking()
  {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
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
  void RenderMenuBar()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Open"))
        {
          CONSOLE_TRACE("Open...");
        }
        if (ImGui::MenuItem("Save"))
        {
          CONSOLE_TRACE("Save...");
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }
  GameObject RenderOutlinerPanel(Scene& scene)
  {
    static char nodeName[64]{};
    static GameObject objectSelected{};
    
    static bool visible = true;
    if (visible)
    {
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin("Outliner", &visible);
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      if (ImGui::TreeNode("Scene"))
      {
        for (auto [entity, lComp] : scene.Reg().view<LabelComponent>().each())
        {
          GameObject object{ entity, &scene.Reg() };

          ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_Leaf |
            ImGuiTreeNodeFlags_NoTreePushOnOpen;

          if (objectSelected && objectSelected.IsEqual(object))
            flags |= ImGuiTreeNodeFlags_Selected;

          std::format_to(nodeName, "{}", lComp.label.c_str());
          ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, nodeName);

          if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            objectSelected = object;

          std::fill_n(nodeName, 64, 0);
        }
        ImGui::TreePop();
      }
      ImGui::End();
    }

    return objectSelected;
  }
  void RenderDetails(GameObject object)
  {
    static bool visible = true;
    constexpr float infinity = 1'000'000;

    if (!visible)
      return;

    ImGui::Begin("Details", &visible);
    
    if (auto light = object.GetComponent<DirLightComponent>())
    {
      if (ImGui::CollapsingHeader("DirLightComponent"))
      {
        ImGui::ColorEdit3("color", (float*)&light->color);
        ImGui::SliderFloat("ambient", &light->ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuse", &light->diffuse,  0.0f, 1.0f);
        ImGui::SliderFloat("specular", &light->specular, 0.0f, 1.0f);
        ImGui::DragFloat3("direction", (float*)&light->direction, 0.1f, -infinity, infinity);
      }
    }
    if (auto light = object.GetComponent<PointLightComponent>())
    {
      if (ImGui::CollapsingHeader("PointLightComponent"))
      {
        ImGui::ColorEdit3("color", (float*)&light->color);
        ImGui::SliderFloat("ambient", &light->ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuse", &light->diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("specular", &light->specular, 0.0f, 1.0f);
        ImGui::DragFloat3("position", (float*)&light->position, 0.1f, -infinity, infinity);
        ImGui::SliderFloat("linear", &light->linear, 0.0f, 1.0f);
        ImGui::SliderFloat("quadratic", &light->quadratic, 0.0f, 1.0f);
      }
    }
    if (auto light = object.GetComponent<SpotLightComponent>())
    {
      if (ImGui::CollapsingHeader("SpotLightComponent"))
      {
        ImGui::ColorEdit3("color", (float*)&light->color);
        ImGui::SliderFloat("ambient", &light->ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuse", &light->diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("specular", &light->specular, 0.0f, 1.0f);
        ImGui::DragFloat3("direction", (float*)&light->direction, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("position", (float*)&light->position, 0.1f, -infinity, infinity);
        ImGui::SliderFloat("linear", &light->linear, 0.0f, 1.0f);
        ImGui::SliderFloat("quadratic", &light->quadratic, 0.0f, 1.0f);
        ImGui::SliderFloat("cutoff", &light->cutOff, 1.0f, light->outerCutOff);
        ImGui::SliderFloat("outer cutoff", &light->outerCutOff, light->cutOff, 30.0f);
      }
    }
    if (auto transform = object.GetComponent<TransformComponent>())
    {
      if (ImGui::CollapsingHeader("TransformComponent"))
      {
        ImGui::DragFloat3("position", (float*)&transform->position, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("scale", (float*)&transform->scale, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("rotation", (float*)&transform->rotation, 0.1f, -180.0f, 180.0f);
        transform->UpdateTransformation();
      }
    }
    if (auto mesh = object.GetComponent<StaticMeshComponent>())
    {
      if (ImGui::CollapsingHeader("StaticMeshComponent"))
      {
        // VertexArray vao;
        // Material		material;
        // fspath			modelPath;
      }
    }
    
    ImGui::End();
  }
  void RenderDemo()
  {
    static bool visible = true;
    if (!visible)
      return;
    
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::ShowDemoWindow(&visible);
  }
}