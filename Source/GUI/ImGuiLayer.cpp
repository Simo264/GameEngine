#include "ImGuiLayer.hpp"

#include "Core/Math/Extensions.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Camera.hpp"
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

static constexpr int infinity = 1'000'000;
static int guizmode = ImGuizmo::OPERATION::TRANSLATE;

static const char* ATTENUATION_LABELS[]{
  "7m",
  "13m",
  "20m",
  "32m",
  "50m",
  "65m",
  "100m",
  "160m",
  "200m",
  "325m",
  "600m",
  "3250m"
};
static const array<tuple<float, float>, 12> ATTENUATION_VALUES = {
  std::make_tuple<float,float>(0.7f, 1.8f),         // 7 meters
  std::make_tuple<float,float>(0.35f, 0.44f),       // 13 meters
  std::make_tuple<float,float>(0.22f, 0.20f),       // 20 meters
  std::make_tuple<float,float>(0.14f, 0.07f),       // 32 meters
  std::make_tuple<float,float>(0.09f, 0.032f),      // 50 meters
  std::make_tuple<float,float>(0.07f, 0.017f),      // 65 meters
  std::make_tuple<float,float>(0.045f, 0.0075f),    // 100 meters
  std::make_tuple<float,float>(0.027f, 0.0028f),    // 160 meters
  std::make_tuple<float,float>(0.022f, 0.0019f),    // 200 meters
  std::make_tuple<float,float>(0.014f, 0.0007f),    // 325 meters
  std::make_tuple<float,float>(0.007f, 0.0002f),    // 600 meters
  std::make_tuple<float,float>(0.0014f, 0.000007f), // 3250 meters
};

static void GuizmoWorldTranslation(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
static void GuizmoWorldRotation(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
static void GuizmoWorldScaling(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
    
    ImGui_ImplGlfw_InitForOpenGL(g_windowManager.GetCurrentContext(), true);
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
    ImGui::RenderPanel();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      WindowManager::Context backCurrentContext = g_windowManager.GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      g_windowManager.MakeContextCurrent(backCurrentContext);
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
            for (const auto& program : g_shaderManager.programs)
              program.Link();
            
            g_shaderManager.SetUpProgramsUniforms();

            scene.ClearScene();
            scene.LoadScene(filePath);
            CONSOLE_INFO("New scene has been loaded");
          }
        }
        if (ImGui::MenuItem("Save as..."))
        {
          static const char* filters[1] = { "*.ini" };
          fspath filepath = FileDialog::SaveFileDialog(1, filters, "Save scene");

          scene.SaveScene(filepath);
          CONSOLE_TRACE("The scene has been saved");
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }
  vec2i32 RenderViewportAndGuizmo(const Texture2D& image, GameObject& object, const mat4f& view, const mat4f& proj)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 paddingTmp = style.WindowPadding;
    style.WindowPadding= { 0.0f, 0.0f };
    
    ImGui::Begin("Viewport", nullptr);
    ImGui::BeginChild("GameRender");
    
    const ImVec2 viewport = ImGui::GetWindowSize();
    
    ImGui::Image(reinterpret_cast<ImTextureID>(image.id), viewport, ImVec2(0, 1), ImVec2(1, 0));

    if (object.IsValid())
    {
      auto* transform = object.GetComponent<Components::Transform>();
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

    return vec2i32(viewport.x, viewport.y);
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
        for (auto [entity, label] : scene.Reg().view<Components::Label>().each())
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

          std::format_to(nodeName, "{}", label.label.c_str());
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

    if (!visible)
      return;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Details", &visible);
    
    if (auto light = object.GetComponent<Components::DirectionalLight>())
    {
      if (ImGui::CollapsingHeader("Directional light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (float*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (float*)&light->direction, 0.1f, -infinity, infinity);
      }
    }
    if (auto light = object.GetComponent<Components::PointLight>())
    {
      if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (float*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Position", (float*)&light->position, 0.1f, -infinity, infinity);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static int index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }
      }
    }
    if (auto light = object.GetComponent<Components::SpotLight>())
    {
      if (ImGui::CollapsingHeader("Spot light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (float*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (float*)&light->direction, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Position", (float*)&light->position, 0.1f, -infinity, infinity);
        
        ImGui::Text("Radius");
        ImGui::Separator();
        ImGui::SliderFloat("Inner cutoff", &light->cutOff, 1.0f, light->outerCutOff);
        ImGui::SliderFloat("Outer cutoff", &light->outerCutOff, light->cutOff, 45.0f);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static int index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }
      }
    }
    if (auto transform = object.GetComponent<Components::Transform>())
    {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::RadioButton("Translate", &guizmode, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::RadioButton("Rotate", &guizmode, ImGuizmo::OPERATION::ROTATE);
        ImGui::RadioButton("Scale", &guizmode, ImGuizmo::OPERATION::SCALE);

        ImGui::DragFloat3("Position", (float*)&transform->position, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Scale", (float*)&transform->scale, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Rotation", (float*)&transform->rotation, 0.1f, -infinity, infinity);
        transform->UpdateTransformation();
      }
    }
    
    ImGui::End();
  }
  void RenderGlobals()
  {
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Globals", nullptr);

    if (ImGui::CollapsingHeader("Draw mode"))
    {
      ImGui::RadioButton("GL_POINTS", &g_drawMode, GL_POINTS);
      ImGui::RadioButton("GL_LINES", &g_drawMode, GL_LINES);
      ImGui::RadioButton("GL_LINE_LOOP", &g_drawMode, GL_LINE_LOOP);
      ImGui::RadioButton("GL_LINE_STRIP", &g_drawMode, GL_LINE_STRIP);
      ImGui::RadioButton("GL_TRIANGLES", &g_drawMode, GL_TRIANGLES);
      ImGui::RadioButton("GL_TRIANGLE_STRIP", &g_drawMode, GL_TRIANGLE_STRIP);
      ImGui::RadioButton("GL_TRIANGLE_FAN", &g_drawMode, GL_TRIANGLE_FAN);
      ImGui::RadioButton("GL_QUADS", &g_drawMode, GL_QUADS);
      ImGui::RadioButton("GL_QUAD_STRIP", &g_drawMode, GL_QUAD_STRIP);
      ImGui::RadioButton("GL_POLYGON", &g_drawMode, GL_POLYGON);
    }
    if (ImGui::CollapsingHeader("World"))
    {
      ImGui::ColorEdit3("Ambient color", (float*)&g_ambientColor);
      ImGui::SliderFloat("Ambient intensity", &g_ambientIntensity, 0.0f, 1.0f);
    }
    
    ImGui::End();
  }

  void RenderDepthMap(const Texture2D& image)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 paddingTmp = style.WindowPadding;
    style.WindowPadding = { 0.0f, 0.0f };

    ImGui::Begin("Depth map", nullptr);
    ImGui::BeginChild("Map");

    ImGui::Image(reinterpret_cast<ImTextureID>(image.id), { 1024,1024 }, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::EndChild();
    ImGui::End();

    style.WindowPadding = paddingTmp;
  }

  void RenderDebug(Camera& camera)
  {
    ImGui::Begin("Debug", nullptr);

    ImGui::DragFloat("zNear", &camera.frustum.zNear, 0.1f, 0.1f, 200.0f);
    ImGui::DragFloat("zFar", &camera.frustum.zFar, 0.1f, 0.1f, 100.0f);
    ImGui::DragFloat("Left", &camera.frustum.left, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat("Right", &camera.frustum.right, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat("Top", &camera.frustum.top, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat("Bottom", &camera.frustum.bottom, 0.1f, -100.0f, 100.0f);

    ImGui::DragFloat("Yaw", &camera.yaw, 0.1f, -180.0f, 180.0f);
    ImGui::DragFloat("Pitch", &camera.pitch, 0.1f, -180.0f, 180.0f);
    ImGui::DragFloat("Roll", &camera.roll, 0.1f, -180.0f, 180.0f);
    ImGui::DragFloat3("Position", (float*)&camera.position, 0.1f, -1000.0f, 1000.0f);

    ImGui::End();
  }
}