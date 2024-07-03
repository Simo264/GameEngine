#include "ImGuiLayer.hpp"

#include "Core/Math/Extensions.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

#include <GLFW/glfw3.h>

extern vec3f lightPosition;

static int guizmode = ImGuizmo::OPERATION::TRANSLATE;

static void GuizmoWorldTranslation(TransformComponent& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::TRANSLATE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    Quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    transform.position = translation;
    transform.UpdateTransformation();
  }
}
static void GuizmoWorldRotation(TransformComponent& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::ROTATE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    Quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    vec3f rotationDegrees = Math::EulerAngles(rotation);  /* Get vector rotation in radians */
    rotationDegrees.x = Math::Degrees(rotationDegrees.x); /* Convert it in degrees */
    rotationDegrees.y = Math::Degrees(rotationDegrees.y);
    rotationDegrees.z = Math::Degrees(rotationDegrees.z);
    const vec3f deltaRotation = rotationDegrees - transform.rotation;

    transform.rotation += deltaRotation;
    transform.UpdateTransformation();
  }
}
static void GuizmoWorldScaling(TransformComponent& transform, const mat4f& view, const mat4f& proj)
{
  mat4f& model = transform.GetTransformation();
  ImGuizmo::Manipulate(
    &view[0][0],
    &proj[0][0],
    ImGuizmo::OPERATION::SCALE,
    ImGuizmo::WORLD,
    &model[0][0]);

  if (ImGuizmo::IsUsing())
  {
    vec3f translation;
    vec3f scale;
    Quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    transform.scale = scale;
    transform.UpdateTransformation();
  }
}

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

  void BeginFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::Begin();
    ImGuizmo::BeginFrame();
  }
  void EndFrame()
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
  void RenderDemo()
  {
    static bool visible = true;
    if (!visible)
      return;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::ShowDemoWindow(&visible);
  }
  void RenderMenuBar(Scene& scene)
  {
    if(ImGui::BeginMainMenuBar())
    {
      if(ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Open"))
        {
          static const char* filters[1] = { "*.ini" };
          fspath filePath = FileDialog::OpenFileDialog(1, filters, "Open scene (.ini)", false);

          if (!filePath.empty())
          {
            /* !IMPORTANT: before loading new scene it needed to relink all the programs to see changes */
            auto instanceSM = ShaderManager::Instance();
            for (const auto& program : instanceSM->programs)
              program.Link();
            
            instanceSM->SetUpProgramsUniforms();

            scene.ClearScene();
            scene.LoadScene(filePath);
            CONSOLE_INFO("New scene has been loaded");
          }
        }
        if (ImGui::MenuItem("Save"))
        {
          scene.SaveScene(ROOT_PATH / "Scene.ini");
          CONSOLE_TRACE("The scene has been saved");
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }
  void RenderViewportAndGuizmo(const Texture2D& image, GameObject& object, const mat4f& view, const mat4f& proj)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 paddingTmp = style.WindowPadding;
    style.WindowPadding= { 0.0f, 0.0f };
    
    ImGui::Begin("Viewport", nullptr);
    ImGui::BeginChild("GameRender");

    const ImVec2 size = ImGui::GetWindowSize();
    ImGui::Image(reinterpret_cast<ImTextureID>(image.id), size, ImVec2(0, 1), ImVec2(1, 0));

    if (object.IsValid())
    {
      TransformComponent* transform = object.GetComponent<TransformComponent>();
      if (transform)
      {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowW = ImGui::GetWindowWidth();
        float windowH = ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowW, windowH);

        switch (guizmode)
        {
        case ImGuizmo::OPERATION::TRANSLATE:
          GuizmoWorldTranslation(*transform, view, proj);
          break;

        case ImGuizmo::OPERATION::ROTATE:
          GuizmoWorldRotation(*transform, view, proj);
          break;

        case ImGuizmo::OPERATION::SCALE:
          GuizmoWorldScaling(*transform, view, proj);
          break;

        default:
          break;
        }
      }
    }
    
    ImGui::EndChild();
    ImGui::End();

    style.WindowPadding = paddingTmp;
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

          if (objectSelected.IsValid() && objectSelected.IsEqual(object))
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

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Details", &visible);
    
    if (auto light = object.GetComponent<DirLightComponent>())
    {
      if (ImGui::CollapsingHeader("DirLightComponent", ImGuiTreeNodeFlags_DefaultOpen))
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
      if (ImGui::CollapsingHeader("PointLightComponent", ImGuiTreeNodeFlags_DefaultOpen))
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
      if (ImGui::CollapsingHeader("SpotLightComponent", ImGuiTreeNodeFlags_DefaultOpen))
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
      if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::RadioButton("Translate", &guizmode, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::RadioButton("Rotate", &guizmode, ImGuizmo::OPERATION::ROTATE);
        ImGui::RadioButton("Scale", &guizmode, ImGuizmo::OPERATION::SCALE);

        ImGui::DragFloat3("position", (float*)&transform->position, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("scale", (float*)&transform->scale, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("rotation", (float*)&transform->rotation, 0.1f, -180.0f, 180.0f);
        transform->UpdateTransformation();
      }
    }
    
    ImGui::End();
  }
  void RenderTesting()
  {
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Testing", nullptr);

    if (ImGui::CollapsingHeader("Draw mode"))
    {
      ImGui::RadioButton("GL_POINTS", &DRAW_MODE, GL_POINTS);
      ImGui::RadioButton("GL_LINES", &DRAW_MODE, GL_LINES);
      ImGui::RadioButton("GL_LINE_LOOP", &DRAW_MODE, GL_LINE_LOOP);
      ImGui::RadioButton("GL_LINE_STRIP", &DRAW_MODE, GL_LINE_STRIP);
      ImGui::RadioButton("GL_TRIANGLES", &DRAW_MODE, GL_TRIANGLES);
      ImGui::RadioButton("GL_TRIANGLE_STRIP", &DRAW_MODE, GL_TRIANGLE_STRIP);
      ImGui::RadioButton("GL_TRIANGLE_FAN", &DRAW_MODE, GL_TRIANGLE_FAN);
      ImGui::RadioButton("GL_QUADS", &DRAW_MODE, GL_QUADS);
      ImGui::RadioButton("GL_QUAD_STRIP", &DRAW_MODE, GL_QUAD_STRIP);
      ImGui::RadioButton("GL_POLYGON", &DRAW_MODE, GL_POLYGON);
    }
    if (ImGui::CollapsingHeader("Shadow map"))
    {
      ImGui::SliderFloat("Z_NEAR", &Z_NEAR, 0.0f, 1.0f);
      ImGui::SliderFloat("Z_FAR", &Z_FAR, 0.0f, 500.0f);
      ImGui::SliderFloat("LEFT", &LEFT, 0.0f, -100.0f);
      ImGui::SliderFloat("RIGHT", &RIGHT, 0.0f, 100.0f);
      ImGui::SliderFloat("BOTTOM", &BOTTOM, 0.0f, -100.0f);
      ImGui::SliderFloat("TOP", &TOP, 0.0f, 100.0f);
    }

    ImGui::DragFloat3("Light position", &lightPosition[0], 0.1f, -100, 100);
    
    ImGui::End();
  }
}