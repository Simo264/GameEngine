#include "ImGuiLayer.hpp"

#include "Core/GL.hpp"

#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Camera.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/Filesystem/Dialog.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

#include <GLFW/glfw3.h>

static constexpr i32 infinity = 1'000'000;
static i32 guizmode = ImGuizmo::OPERATION::TRANSLATE;

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
static const Array<Tuple<f32, f32>, 12> ATTENUATION_VALUES = {
  std::make_tuple<f32,f32>(0.7f, 1.8f),         // 7 meters
  std::make_tuple<f32,f32>(0.35f, 0.44f),       // 13 meters
  std::make_tuple<f32,f32>(0.22f, 0.20f),       // 20 meters
  std::make_tuple<f32,f32>(0.14f, 0.07f),       // 32 meters
  std::make_tuple<f32,f32>(0.09f, 0.032f),      // 50 meters
  std::make_tuple<f32,f32>(0.07f, 0.017f),      // 65 meters
  std::make_tuple<f32,f32>(0.045f, 0.0075f),    // 100 meters
  std::make_tuple<f32,f32>(0.027f, 0.0028f),    // 160 meters
  std::make_tuple<f32,f32>(0.022f, 0.0019f),    // 200 meters
  std::make_tuple<f32,f32>(0.014f, 0.0007f),    // 325 meters
  std::make_tuple<f32,f32>(0.007f, 0.0002f),    // 600 meters
  std::make_tuple<f32,f32>(0.0014f, 0.000007f), // 3250 meters
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
    quat  rotation;
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
    quat  rotation;
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
    quat  rotation;
    Math::Decompose(model, translation, rotation, scale);

    transform.scale = scale;
    transform.UpdateTransformation();
  }
}

static void ComboTextures(Texture2D*& matTexture, const char* comboLabel)
{
  const char* comboPreview = "Select texture";
  if (ImGui::BeginCombo(comboLabel, comboPreview))
  {
    TextureManager& texManager = TextureManager::Get();
    const auto& textVector = texManager.GetTextureVector();

    for (i32 i = 0; i < textVector.size(); i++)
    {
      Texture2D* texture = texManager.GetTextureAt(i);
      String texPathStr = texture->path.string();
      if (!texPathStr.empty() && ImGui::Selectable(texPathStr.c_str(), false))
        matTexture = texture;
    }
    ImGui::EndCombo();
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
    
    ImGui_ImplGlfw_InitForOpenGL(WindowManager::Get().GetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
  }
  
  void CleanUp()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
      WindowManager& windowManager = WindowManager::Get();
      WindowManager::Context backCurrentContext = windowManager.GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      windowManager.MakeContextCurrent(backCurrentContext);
    }
  }

  void SetFont(const fs::path& fontpath, i32 fontsize)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(fontpath.string().c_str(), fontsize);
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
  
  void Demo()
  {
    static bool visible = true;
    if (!visible)
      return;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::ShowDemoWindow(&visible);
  }
  
  void MenuBar(Scene& scene)
  {
    if(ImGui::BeginMainMenuBar())
    {
      if(ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Open"))
        {
          static const char* filters[1] = { "*.ini" };
          fs::path filePath = OpenFileDialog(1, filters, "Open scene (.ini)", false);

          if (!filePath.empty())
          {
            ShaderManager& shaderManager = ShaderManager::Get();

            /* !IMPORTANT: before loading new scene it needed to relink all the programs to see changes */
            for (const auto& program : shaderManager.GetProgramsVector())
              program.Link();
            
            shaderManager.SetUpProgramsUniforms();

            scene.ClearScene();
            scene.LoadScene(filePath);
            CONSOLE_INFO("New scene has been loaded");
          }
        }
        if (ImGui::MenuItem("Save as..."))
        {
          static const char* filters[1] = { "*.ini" };
          fs::path filepath = SaveFileDialog(1, filters, "Save scene");

          scene.SaveScene(filepath);
          CONSOLE_TRACE("The scene has been saved");
        }
        
        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
        {
          WindowManager::Get().Close();
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
  }
  
  vec2i32 ViewportGizmo(u32 tetxureID, GameObject& object, const mat4f& view, const mat4f& proj)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 paddingTmp = style.WindowPadding;
    style.WindowPadding= { 0.0f, 0.0f };
    
    ImGui::Begin("Viewport", nullptr);
    ImGui::BeginChild("GameRender");
    
    const ImVec2 viewport = ImGui::GetWindowSize();
    
    ImGui::Image(reinterpret_cast<void*>(tetxureID), viewport, ImVec2(0, 1), ImVec2(1, 0));

    if (object.IsValid())
    {
      auto* transform = object.GetComponent<Components::Transform>();
      if (transform)
      {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        f32 windowW = ImGui::GetWindowWidth();
        f32 windowH = ImGui::GetWindowHeight();
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

  GameObject OutlinerPanel(Scene& scene)
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

          std::format_to(nodeName, "{}", label.value.c_str());
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
  
  void GameObjectDetails(GameObject& object)
  {
    static bool visible = true;

    if (!object.IsValid())
      return;

    if (!visible)
      return;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Details", &visible);
    
    if (auto* light = object.GetComponent<Components::DirectionalLight>())
    {
      if (ImGui::CollapsingHeader("Directional light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (f32*)&light->direction, 0.1f, -infinity, infinity);
      }
    }
    if (auto* light = object.GetComponent<Components::PointLight>())
    {
      if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Position", (f32*)&light->position, 0.1f, -infinity, infinity);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static i32 index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }
      }
    }
    if (auto* light = object.GetComponent<Components::SpotLight>())
    {
      if (ImGui::CollapsingHeader("Spot light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (f32*)&light->direction, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Position", (f32*)&light->position, 0.1f, -infinity, infinity);
        
        ImGui::Text("Radius");
        ImGui::Separator();
        ImGui::SliderFloat("Inner cutoff", &light->cutOff, 1.0f, light->outerCutOff);
        ImGui::SliderFloat("Outer cutoff", &light->outerCutOff, light->cutOff, 45.0f);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static i32 index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }
      }
    }
    if (auto* transform = object.GetComponent<Components::Transform>())
    {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::RadioButton("Translate", &guizmode, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::RadioButton("Rotate", &guizmode, ImGuizmo::OPERATION::ROTATE);
        ImGui::RadioButton("Scale", &guizmode, ImGuizmo::OPERATION::SCALE);

        ImGui::DragFloat3("Position", (f32*)&transform->position, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Scale", (f32*)&transform->scale, 0.1f, -infinity, infinity);
        ImGui::DragFloat3("Rotation", (f32*)&transform->rotation, 0.1f, -infinity, infinity);
        transform->UpdateTransformation();
      }
    }
    if (auto* model = object.GetComponent<Components::Model>())
    {
      if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
      {
        /* Display model path */
        ImGui::TextWrapped("Path: %s", model->path.string().c_str());
        ImGui::Separator();

        /* Display the number of meshes */
        const i64 numMeshes = model->meshes.size();
        ImGui::TextWrapped("Meshes: %d", numMeshes);
        
        /* For each mesh */
        for (i32 i = 0; i < numMeshes; i++)
        {
          auto& mesh = model->meshes.at(i);
          Material& material = mesh.material;

          /* Display mesh-i props */
          char title[16]{};
          std::format_to_n(title, sizeof(title), "Mesh {}", i);
          ImGui::SeparatorText(title);
          
          const auto& diffPath = material.diffuse->path;
          ImGui::TextWrapped("Diffuse: %s", (diffPath.empty() ? "None" : diffPath.string().c_str()));
          ComboTextures(material.diffuse, "##diffuse");
          if (!diffPath.empty() && ImGui::Button("Reset##diffuse"))
            material.diffuse = TextureManager::Get().GetTextureAt(0);
          ImGui::Separator();
          
          const auto& specularPath = material.specular->path;
          ImGui::TextWrapped("Specular: %s", (specularPath.empty() ? "None" : specularPath.string().c_str()));
          ComboTextures(material.specular, "##specular");
          if (!specularPath.empty() && ImGui::Button("Reset##specular"))
            material.specular = TextureManager::Get().GetTextureAt(1);
          ImGui::Separator();
          
          const auto& normalPath = material.normal->path;
          ImGui::TextWrapped("Normal: %s", (normalPath.empty() ? "None" : normalPath.string().c_str()));
          ComboTextures(material.normal, "##normal");
          if (!normalPath.empty() && ImGui::Button("Reset##normal"))
            material.normal = TextureManager::Get().GetTextureAt(2);
          ImGui::Separator();

          const auto& heightPath = material.height->path;
          ImGui::TextWrapped("Height: %s", (heightPath.empty() ? "None" : heightPath.string().c_str()));
          ComboTextures(material.height, "##height");
          if (!heightPath.empty() && ImGui::Button("Reset##height"))
            material.height = TextureManager::Get().GetTextureAt(3);
          ImGui::Separator();
        }
      }
    }

    ImGui::End();
  }
  
  void WorldProps()
  {
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("World", nullptr);

    if (ImGui::CollapsingHeader("Ambient"))
    {
      ImGui::ColorEdit3("Light color", (f32*)&g_ambientColor);
      ImGui::SliderFloat("Light intensity", &g_ambientIntensity, 0.0f, 1.0f);
    }
    
    ImGui::End();
  }
  
  void DebugDepthMap(u32 tetxureID)
  {
    ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 paddingTmp = style.WindowPadding;
    style.WindowPadding = { 0.0f, 0.0f };

    ImGui::Begin("Depth map", nullptr);
    ImGui::BeginChild("Map");

    ImGui::Image(reinterpret_cast<void*>(tetxureID), { 1024,1024 }, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::EndChild();
    ImGui::End();

    style.WindowPadding = paddingTmp;
  }
  
  void CameraProps(const char* label, Camera& camera)
  {
    ImGui::Begin(label, nullptr);

    ImGui::DragFloat("zNear", &camera.frustum.zNear, 0.1f, 0.1f, 200.0f);
    ImGui::DragFloat("zFar", &camera.frustum.zFar, 0.1f, 0.1f, 200.0f);
    ImGui::DragFloat("Left", &camera.frustum.left, 0.1f, -100.0f, 0.0f);
    ImGui::DragFloat("Right", &camera.frustum.right, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Top", &camera.frustum.top, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Bottom", &camera.frustum.bottom, 0.1f, -100.0f, 0.0);

    ImGui::DragFloat("Yaw", &camera.yaw, 0.1f, -360.0f, 360);
    ImGui::DragFloat("Pitch", &camera.pitch, 0.1f, -360, 360);
    ImGui::DragFloat("Roll", &camera.roll, 0.1f, -360, 360.0f);
    ImGui::DragFloat3("Position", (f32*)&camera.position, 0.1f, -infinity, infinity);

    ImGui::DragFloat("Fov", &camera.fov, 0.1f, 0.0f, 180.0f);

    ImGui::End();
  }

  void ApplicationInfo(f64 delta, f64 avg, i32 frameRate)
  {
    ImGui::Begin("Application", nullptr);

    auto& winManager = WindowManager::Get();
    static const char* glRender = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    static const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    static const char* glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    static const char* glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::TextWrapped("GLFW: %s\nOpenGL renderer: %s\nOpenGL version: %s\nOpenGL vendor: %s\nOpenGL Shading Language Version: %s",
      winManager.GetVersion(),
      glRender,
      glVersion,
      glVendor,
      glsl
    );

    ImGui::Separator();

    ImGui::TextWrapped("Time (ms): %f", delta*1000);
    ImGui::TextWrapped("Average (ms): %f", avg*1000);
    ImGui::TextWrapped("Frame rate: %d", frameRate);

    ImGui::End();
  }

  void Test()
  {
    ImGui::Begin("Test", nullptr);

    ImGui::End();
  }
}