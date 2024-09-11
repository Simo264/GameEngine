#include "ImGuiLayer.hpp"

#include "Core/GL.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"
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

constexpr i32 infinity = 1'000'000;
constexpr const char* ATTENUATION_LABELS[]{
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
constexpr Array<Tuple<f32, f32>, 12> ATTENUATION_VALUES = {
  std::make_tuple<f32,f32>(0.7f, 1.8f),         /* 7 meters */ 
  std::make_tuple<f32,f32>(0.35f, 0.44f),       /* 13 meters */
  std::make_tuple<f32,f32>(0.22f, 0.20f),       /* 20 meters */
  std::make_tuple<f32,f32>(0.14f, 0.07f),       /* 32 meters */
  std::make_tuple<f32,f32>(0.09f, 0.032f),      /* 50 meters */
  std::make_tuple<f32,f32>(0.07f, 0.017f),      /* 65 meters */
  std::make_tuple<f32,f32>(0.045f, 0.0075f),    /* 100 meters */
  std::make_tuple<f32,f32>(0.027f, 0.0028f),    /* 160 meters */
  std::make_tuple<f32,f32>(0.022f, 0.0019f),    /* 200 meters */
  std::make_tuple<f32,f32>(0.014f, 0.0007f),    /* 325 meters */
  std::make_tuple<f32,f32>(0.007f, 0.0002f),    /* 600 meters */
  std::make_tuple<f32,f32>(0.0014f, 0.000007f), /* 3250 meters */
};


/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

void ImGuiLayer::SetupContext()
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
void ImGuiLayer::CleanUp()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
void ImGuiLayer::BeginFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::Begin();
  ImGuizmo::BeginFrame();
}
void ImGuiLayer::EndFrame()
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
void ImGuiLayer::SetFont(const fs::path& fontpath, i32 fontsize)
{
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontpath.string().c_str(), fontsize);
}
void ImGuiLayer::Docking()
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
void ImGuiLayer::Demo()
{
  static bool visible = true;
  if (!visible)
    return;

  ImGui::ShowDemoWindow(&visible);
}
void ImGuiLayer::MenuBar(Scene& scene)
{
  if(ImGui::BeginMainMenuBar())
  {
    if(ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open"))
      {
        const char* filters[1] = { "*.ini" };
        fs::path filePath = OpenFileDialog(1, filters, "Open scene", false);

        if (!filePath.empty())
        {
          ShaderManager& shaderManager = ShaderManager::Get();

          /* !IMPORTANT: before loading new scene it needed to relink all the programs to see changes */
          for (const auto& [key, program] : shaderManager.GetPrograms())
            program.Link();
            
          shaderManager.ResetProgramsUniforms();

          scene.ClearScene();
          scene.LoadScene(filePath);
          CONSOLE_INFO("The scene has been successfully loaded");
        }
      }
      if (ImGui::MenuItem("Save as..."))
      {
        const char* filters[1] = { "*.ini" };
        fs::path filepath = SaveFileDialog(1, filters, "Save scene");

        scene.SaveScene(filepath);
        CONSOLE_TRACE("The scene has been successfully saved");
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
void ImGuiLayer::ViewportGizmo(u32 tetxureID, GameObject& object, const mat4f& view, const mat4f& proj)
{
  ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 paddingTmp = style.WindowPadding;
  style.WindowPadding= { 0.0f, 0.0f };
  
  /* Begin main viewport */
  ImGui::Begin("Viewport", nullptr);
  const ImVec2 viewportWinSize = ImGui::GetWindowSize();
  const ImVec2 viewportWinPos = ImGui::GetWindowPos();
  viewportSize = { viewportWinSize.x, viewportWinSize.y };
  viewportPos = { viewportWinPos.x, viewportWinPos.y };
  viewportFocused = ImGui::IsWindowFocused();
  
  /* Being child viewport */
  ImGui::BeginChild("Viewport_Child");
  viewportFocused = viewportFocused || ImGui::IsWindowFocused();

  const ImVec2 viewportChildWinSize = ImGui::GetWindowSize();
  ImGui::Image(reinterpret_cast<void*>(tetxureID), viewportChildWinSize, ImVec2(0, 1), ImVec2(1, 0));
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

      switch (_gizmode)
      {
      case ImGuizmo::OPERATION::TRANSLATE:
        GizmoWorldTranslation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::ROTATE:
        GizmoWorldRotation(*transform, view, proj);
        break;

      case ImGuizmo::OPERATION::SCALE:
        GizmoWorldScaling(*transform, view, proj);
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
GameObject ImGuiLayer::OutlinerPanel(Scene& scene)
{
  static char nodeName[64]{};
  static GameObject objectSelected{};
    
  static bool visible = true;
  if (visible)
  {
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
void ImGuiLayer::GameObjectDetails(GameObject& object)
{
  static bool visible = true;

  if (!object.IsValid())
    return;

  if (!visible)
    return;

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
      ImGui::RadioButton("Translate", &_gizmode, ImGuizmo::OPERATION::TRANSLATE);
      ImGui::RadioButton("Rotate", &_gizmode, ImGuizmo::OPERATION::ROTATE);
      ImGui::RadioButton("Scale", &_gizmode, ImGuizmo::OPERATION::SCALE);

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
        TextureManager& texManager = TextureManager::Get();

        /* Display mesh-i props */
        char title[16]{};
        std::format_to_n(title, sizeof(title), "Mesh {}", i);
        ImGui::SeparatorText(title);
          
        const String& diffPath = material.diffuse->strPath;
        bool isTextureEmpty = (diffPath.at(0) == '#');
        ImGui::TextWrapped("Diffuse: %s", isTextureEmpty ? "None" : diffPath.c_str());
        ComboTextures(material.diffuse, "##diffuse");
        if (!isTextureEmpty && ImGui::Button("Reset##diffuse"))
          material.diffuse = &texManager.GetTextureByPath("#default_diffuse");
        ImGui::Separator();
          
        const String& specularPath = material.specular->strPath;
        isTextureEmpty = (specularPath.at(0) == '#');
        ImGui::TextWrapped("Specular: %s", isTextureEmpty ? "None" : specularPath.c_str());
        ComboTextures(material.specular, "##specular");
        if (!isTextureEmpty && ImGui::Button("Reset##specular"))
          material.specular = &texManager.GetTextureByPath("#default_specular");
        ImGui::Separator();
          
        const String& normalPath = material.normal->strPath;
        isTextureEmpty = (normalPath.at(0) == '#');
        ImGui::TextWrapped("Normal: %s", isTextureEmpty ? "None" : normalPath.c_str());
        ComboTextures(material.normal, "##normal");
        if (!isTextureEmpty && ImGui::Button("Reset##normal"))
          material.normal = &texManager.GetTextureByPath("#default_normal");
        ImGui::Separator();

        const String& heightPath = material.height->strPath;
        isTextureEmpty = (heightPath.at(0) == '#');
        ImGui::TextWrapped("Height: %s", isTextureEmpty ? "None" : heightPath.c_str());
        ComboTextures(material.height, "##height");
        if (!isTextureEmpty && ImGui::Button("Reset##height"))
          material.height = &texManager.GetTextureByPath("#default_height");
        ImGui::Separator();
      }
    }
  }

  ImGui::End();
}
void ImGuiLayer::WorldProps()
{
  ImGui::Begin("World", nullptr);

  if (ImGui::CollapsingHeader("Ambient"))
  {
    ImGui::ColorEdit3("Light color", (f32*)&g_ambientColor);
    ImGui::SliderFloat("Light intensity", &g_ambientIntensity, 0.0f, 1.0f);
  }
    
  ImGui::End();
}
void ImGuiLayer::ContentBrowser()
{
  ImGui::Begin("Content browser", nullptr);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
  
  TextureManager& texManager = TextureManager::Get();

  /* Back button */
  static fs::path currentDir = GetResourcePath();
  if (currentDir != GetResourcePath())
  {
    const auto& iconBack = texManager.GetIconByPath(GetIconsPath() / "back-arrow.png");
    if (ImGui::ImageButton("Back", reinterpret_cast<void*>(iconBack.id), ImVec2(16.0f, 16.0f)))
      currentDir = currentDir.parent_path();
  }

  /* Configure table */
  constexpr f32 cellSize = 64.0f;
  constexpr f32 cellPadding = 16.0f;
  const f32 panelWidth = ImGui::GetContentRegionAvail().x;
  const i32 nColumns = std::max(1, static_cast<i32>(panelWidth / (cellSize + cellPadding))) ;
  if (ImGui::BeginTable("Thumbnails", nColumns, ImGuiTableFlags_SizingStretchSame))
  {
    fs::directory_iterator entryIt = fs::directory_iterator(currentDir);
    const i32 nItemsInCurrentDir = std::distance(fs::begin(entryIt), fs::end(entryIt));
    const i32 nRows = 1 + (nItemsInCurrentDir / nColumns);
    entryIt = fs::directory_iterator(currentDir);

    for (i32 i = 0; i < nRows; i++)
    {
      ImGui::TableNextRow();
      for (i32 j = 0; j < nColumns && entryIt != fs::end(entryIt); j++, entryIt++)
      {
        ImGui::TableSetColumnIndex(j);

        const fs::directory_entry& entry = *entryIt;
        const fs::path& entryPath = entry.path();
        const String entryPathString = entryPath.string();
        const fs::path filename = entryPath.filename();
        const String filenameString = filename.string();

        if (entry.is_directory())
        {
          Texture2D& iconFolder = texManager.GetIconByPath(GetIconsPath() / "open-folder.png");
          if (ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(iconFolder.id), ImVec2(cellSize, cellSize)))
            currentDir /= entry;
        }
        else
        {
          const fs::path entryFilenameExt = filename.extension();

          /* Render image texture */
          if (entryFilenameExt == ".png" || entryFilenameExt == ".jpg")
          {
            Texture2D* texture = nullptr;
            if(currentDir == GetIconsPath())
              texture = &texManager.GetIconByPath(entryPathString);
            else
              texture = &texManager.GetTextureByPath(entryPathString);

            ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(texture->id), ImVec2(cellSize, cellSize));
          }
          /* Render generic file icon */
          else
          {
            const auto& iconFile = texManager.GetIconByPath(GetIconsPath() / "file.png");
            ImGui::ImageButton(filenameString.c_str(), reinterpret_cast<void*>(iconFile.id), ImVec2(cellSize, cellSize));
          }
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
          ImGui::SetTooltip("%s", entryPathString.c_str());
        ImGui::TextWrapped("%s", filenameString.c_str());
      }
    }
    ImGui::EndTable();
  }

  ImGui::PopStyleColor();
  ImGui::End();
}
void ImGuiLayer::DebugDepthMap(u32 tetxureID)
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
void ImGuiLayer::CameraProps(const char* label, Camera& camera)
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
void ImGuiLayer::DebugInfo(f64 delta, f64 avg, i32 frameRate, bool shadowMode, bool normalMode, bool wireframeMode)
{
  ImGuiWindowFlags flags = 
    ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | 
    ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | 
    ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;

  ImGui::SetNextWindowPos(ImVec2(viewportPos.x, viewportPos.y + 20));
  ImGui::SetNextWindowSize(ImVec2(viewportSize.x/2, viewportSize.y - 20));
  ImGui::Begin("Application", nullptr, flags);

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

  ImGui::Separator();

  ImGui::TextWrapped("Shadow mode (F1 on/F2 off): %d", shadowMode);
  ImGui::TextWrapped("Normal mapping (F5 on/F6 off): %d", normalMode);
  ImGui::TextWrapped("Wireframe mode (F9 on/F10 off): %d", wireframeMode);

  ImGui::End();
}
void ImGuiLayer::Test()
{
  ImGui::Begin("Test", nullptr);

  ImGui::End();
}

/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */

ImGuiLayer::ImGuiLayer()
  : viewportFocused{ false },
    viewportSize{},
    viewportPos{},
    _gizmode{ ImGuizmo::OPERATION::TRANSLATE }
{}

void ImGuiLayer::GizmoWorldTranslation(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
void ImGuiLayer::GizmoWorldRotation(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
void ImGuiLayer::GizmoWorldScaling(Components::Transform& transform, const mat4f& view, const mat4f& proj)
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
void ImGuiLayer::ComboTextures(Texture2D*& matTexture, const char* comboLabel)
{
  const char* comboPreview = "Select texture";
  if (ImGui::BeginCombo(comboLabel, comboPreview))
  {
    TextureManager& texManager = TextureManager::Get();
    const auto& textures = texManager.GetTextures();
    for (const auto& [key, texture] : textures)
    {
      const String& texPathStr = texture.strPath;
      if (texPathStr.at(0) != '#' && ImGui::Selectable(texPathStr.c_str(), false))
        matTexture = const_cast<Texture2D*>(&texture);
    }

    ImGui::EndCombo();
  }
}
