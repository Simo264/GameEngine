#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Uniforms.hpp"
#include "Engine/Vertex.hpp"

#include "Engine/Graphics/DepthTest.hpp"
#include "Engine/Graphics/StencilTest.hpp"
#include "Engine/Graphics/FaceCulling.hpp"
#include "Engine/Graphics/Objects/Renderbuffer.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Managers/WindowManager.hpp"
#include "Engine/Managers/ShadersManager.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/StaticMeshFactory.hpp"

#include "GUI/ImGuiLayer.hpp"

using namespace Components;

static constexpr auto INITIAL_WINDOW_W = 1600;
static constexpr auto INITIAL_WINDOW_H = 900;

static void GLAPIENTRY MessageCallback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       [[maybe_unused]] GLsizei length,
                                       const GLchar* message,
                                       [[maybe_unused]] const void* userParam)
{
  // Ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  const char* sourceStr;
  const char* typeStr;
  const char* severityStr;
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    sourceStr = "API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    sourceStr = "Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    sourceStr = "Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    sourceStr = "Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    sourceStr = "Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    sourceStr = "Other";
    break;
  default:
    sourceStr = "Unknown";
    break;
  }

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    typeStr = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    typeStr = "Deprecated Behavior";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    typeStr = "Undefined Behavior";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    typeStr = "Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    typeStr = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    typeStr = "Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    typeStr = "Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    typeStr = "Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    typeStr = "Other";
    break;
  default:
    typeStr = "Unknown";
    break;
  }

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    severityStr = "High";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    severityStr = "Medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    severityStr = "Low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    severityStr = "Notification";
    break;
  default:
    severityStr = "Unknown";
    break;
  }

  CONSOLE_ERROR("GL CALLBACK: {} type = {}, severity = {}, message = {}",
    sourceStr, typeStr, severityStr, message);
}

// -----------------------------------------------------
//                PUBLIC METHODS
// -----------------------------------------------------

void Engine::Initialize()
{
  // 1. Initialize logger
  Logger::Initialize();
  CONSOLE_INFO("Logger initialized");

  // 2. Initialize window manager
  auto props = WindowProps{};
  props.aspectRatio = Vec2I{ 16, 9 };
  props.size = Vec2I{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };
  props.position = Vec2I{ 50, 50 };
  props.title = "GameEngine";
  props.vsync = false;
  props.contextVersionMinor = 4;
  props.contextVersionMajor = 6;
  props.samples = 4;
  WindowManager::GetInstance().Initialize(props);
  CONSOLE_INFO("Window manager initialized");

  // 3. Initialize shader manager
  ShadersManager::GetInstance().Initialize();
  CONSOLE_INFO("Shaders manager initialized");

  // 4. Initialize texture manager
  TexturesManager::GetInstance().Initialize();
  CONSOLE_INFO("Textures manager initialized");

  // 5. Setup ImGui context
  ImGuiLayer::GetInstance().InitializeImGui();
  CONSOLE_INFO("ImGui layer initialized");

  // 6. Set the initial OpenGL states
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // specify clear values for the color buffers
  glClearDepth(1.0f);                       // specify clear values for the depth buffer    
  glClearStencil(0);                        // specify clear values for the stencil buffer 
  
  __SetInitialGLStates();

  // 7. Create the screen quad mesh and framebuffer
  _MSAAFramebufferSamples = 4;
  __CreateMSAAFramebuffer(INITIAL_WINDOW_W, INITIAL_WINDOW_H, _MSAAFramebufferSamples);
  
  // 8. Create the screen quad mesh
  _screenQuad = __CreateMeshQuad();

  // 9. create UBO objects
  __CreateCameraUBO(0); // "CameraBlock" -> 0
  __CreateLightUBO(1);  // "LightBlock" -> 1

  // 10. Initialize time
  __InitTime();
}

void Engine::Run()
{
  auto viewportSize = Vec2I{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };
  auto aspect = static_cast<f32>(viewportSize.x) / viewportSize.y;

  auto scene = Scene();
  
  // initialize object archetypes
  auto& cameraArchetype = Archetype{ "Camera" }.AllowComponents<
    Components::Camera>();
  auto& lightSourceArchetype = Archetype{ "LightSource" }.AllowComponents<
    Components::Light,
    Components::DirectionalLight,
    Components::PointLight,
    Components::SpotLight>();
  auto& staticMeshArchetype = Archetype{ "StaticMesh" }.AllowComponents<
    Components::Transform,
    Components::StaticMesh>();
  scene.RegisterArchetype(cameraArchetype);
  scene.RegisterArchetype(lightSourceArchetype);
  scene.RegisterArchetype(staticMeshArchetype);

  auto obj = scene.CreateObject(cameraArchetype.GetName());
  auto& camera = obj.AddComponent<Components::Camera>();
  camera.position = Vec3F(0.f, 0.f, 10.0f);

  scene.LoadFromFile((Paths::GetRootPath() / "Scene.yaml"));

  //auto obj = scene.FindObjectWithComponent<DirectionalLight>();
  //auto dirLight = obj->GetComponent<DirectionalLight>();
  //obj = scene.FindObjectWithComponent<PointLight>();
  //auto pointLight = obj->GetComponent<PointLight>();
  //obj = scene.FindObjectWithComponent<SpotLight>();
  //auto spotLight = obj->GetComponent<SpotLight>();
  //{
  //  auto& instance = TexturesManager::GetInstance();
  //  
  //  obj = scene.FindObjectWithTag("plane");
  //  auto mesh = obj->GetComponent<StaticMesh>();
  //  mesh->meshArray[0].material.albedo = instance.GetOrCreateTexture("wood_floor_worn/wood_floor_worn_albedo.jpg");
  //  mesh->meshArray[0].material.normalMap = instance.GetOrCreateTexture("wood_floor_worn/wood_floor_worn_normal.jpg");
  //
  //  obj = scene.FindObjectWithTag("monkey");
  //  mesh = obj->GetComponent<StaticMesh>();
  //  mesh->meshArray[0].material.albedo = instance.GetOrCreateTexture("worn-old-plastic/worn-old-plastic_albedo.png");
  //  mesh->meshArray[0].material.normalMap = instance.GetOrCreateTexture("worn-old-plastic/worn-old-plastic_normal-ogl.png");
  //}

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  auto& guiLayer = ImGuiLayer::GetInstance();
  auto& windowManager = WindowManager::GetInstance();
  auto& shadersManager = ShadersManager::GetInstance();
  auto goochProgram = shadersManager.GetProgram("GoochShading");
  auto blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  
  // ------------------------------------------------------------------
  // -------------------------- loop section --------------------------
  // ------------------------------------------------------------------
  while (windowManager.IsOpen())
  {
    guiLayer.PrepareImGuiFrame();

    // ----------------------------------------------------------------------------------
    // -------------------------- Per-frame time logic section --------------------------
    // ----------------------------------------------------------------------------------
    __CalculatePerFrameTime();
    g_DrawCalls = 0u;

    // -------------------------------------------------------------------
    // -------------------------- Input section --------------------------
    // -------------------------------------------------------------------
    windowManager.PoolEvents();
    if (guiLayer.viewportFocused)
    {
      camera.ProcessKeyboard(static_cast<f32>(_delta), 10.0f);
      camera.ProcessMouseMovement(glm::radians(0.05f));
    }

    // --------------------------------------------------------------------
    // -------------------------- Update section --------------------------
    // --------------------------------------------------------------------
    auto cameraView = camera.GetViewMatrix();
    auto cameraProj = camera.GetPerspectiveProjection(glm::radians(45.0f), aspect, 0.1f, 50.f);

    // Update camera UBO
    {
      auto matrices = Array<Mat4F, 2>{ cameraView, cameraProj };
      _uboCameraBlock.UpdateStorage(0, sizeof(matrices), matrices.data());
      _uboCameraBlock.UpdateStorage(sizeof(matrices), sizeof(Vec3F), &camera.position);
    }

    // Update light UBO
    {
      //auto offset = 0;
      //_uboLightBlock.UpdateStorage(offset, sizeof(DirectionalLight), dirLight);
      //offset += sizeof(DirectionalLight);
      //_uboLightBlock.UpdateStorage(offset, sizeof(PointLight), pointLight);
      //offset += sizeof(PointLight);
      //_uboLightBlock.UpdateStorage(offset, sizeof(SpotLight), spotLight);
    }

    // -----------------------------------------------------------------------
    // -------------------------- Rendering section --------------------------
    // -----------------------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, g_RenderInWireframe ? GL_LINE : GL_FILL);
    glViewport(0, 0, viewportSize.x, viewportSize.y);

    // 1. Render on multisampled framebuffer
    _MSAAFramebuffer.Bind(FramebufferTarget::DRAW);
    {
      glEnable(GL_MULTISAMPLE);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers

      blinnPhongProgram.Use();
      scene.GetEntityRegistry().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
        blinnPhongProgram.SetUniformMat4f("u_Transform", transform.GetTransformation());
        staticMesh.Render(blinnPhongProgram, RenderMode::TRIANGLES);
      });

      glDisable(GL_MULTISAMPLE);
    }
    _MSAAFramebuffer.Unbind(FramebufferTarget::DRAW);

    // 2. Resolve framebuffer
    __ResolveMSAAFramebuffer(viewportSize.x, viewportSize.y);

    // 3. Get the final texture color image 
    auto viewportImage = _MSAAFramebufferResolver.GetTextureAttachment(0); // returns the texture color 

    guiLayer.MenuBar(scene);
    guiLayer.ImguiDemo();
    auto objSelected = guiLayer.Hierarchy(scene);
    guiLayer.Inspector(objSelected);
    guiLayer.Viewport(viewportImage, objSelected, cameraView, cameraProj);
    guiLayer.ToolBar();
    guiLayer.TimeInfo(_delta, _avgTime, _frameRate);
    guiLayer.GraphicsInfo();
    guiLayer.CompleteFrameRender();

    // Checking viewport size
    if (viewportSize != guiLayer.viewportSize)
    {
      viewportSize = guiLayer.viewportSize;
      aspect = static_cast<f32>(viewportSize.x) / viewportSize.y;
      
      _MSAAFramebufferResolver.Delete();
      _MSAAFramebuffer.Delete();
      __CreateMSAAFramebuffer(viewportSize.x, viewportSize.y, _MSAAFramebufferSamples);
    }

    // ------------------------------------------------------------------
    // -------------------------- Swap buffers --------------------------
    // ------------------------------------------------------------------
    windowManager.SwapWindowBuffers();
  }
}

void Engine::Cleanup()
{
  _MSAAFramebufferResolver.Delete();
  _MSAAFramebuffer.Delete();
  _uboCameraBlock.Delete();
  _uboLightBlock.Delete();
  _screenQuad.Destroy();

  ImGuiLayer::GetInstance().CleanUpImGui();
  StaticMeshFactory::GetInstance().Cleanup();
  ShadersManager::GetInstance().Cleanup();
  TexturesManager::GetInstance().Cleanup();
  WindowManager::GetInstance().CleanUp();
}


// -----------------------------------------------------
//            PRIVATE METHODS
// -----------------------------------------------------

void Engine::__SetInitialGLStates() const
{
  // Depth testing ON
  // ----------------
  DepthTest::EnableTest();
  DepthTest::EnableWritingBuffer();
  DepthTest::SetDepthFun(CompareFunc::LESS);

  // Stencil testing OFF
  // -------------------
  StencilTest::DisableTest();

  // Culling OFF
  // -----------
  FaceCulling::DisableFaceCulling();

  // Blending OFF
  // ------------
  glDisable(GL_BLEND);

  // Gamma correction OFF
  // --------------------
  glDisable(GL_FRAMEBUFFER_SRGB);

  // Antialising OFF
  // --------------
  glDisable(GL_MULTISAMPLE);
}
void Engine::__InitTime()
{
  _now = chrono::steady_clock::time_point{};
  _lastFrameTime = chrono::steady_clock::time_point{};
  _timerT0 = chrono::steady_clock::now();
  _timerT1 = chrono::steady_clock::time_point{};
  _frames = 0u;
  _frameRate = 0u;
  _totalDeltasPerSecond = 0.0f;
  _avgTime = 0.0f;
  _delta = 0.0f;
}
void Engine::__CalculatePerFrameTime()
{
  _frames++;

  _now = chrono::steady_clock::now();
  _delta = chrono::duration_cast<chrono::duration<f64>>(_now - _lastFrameTime).count();
  _lastFrameTime = _now;
  _totalDeltasPerSecond += _delta;
  _timerT1 = chrono::steady_clock::now();
  auto timer_diff = chrono::duration_cast<chrono::duration<f64>>(_timerT1 - _timerT0).count();
  if (timer_diff >= 1.f)
  {
    _timerT0 = chrono::steady_clock::now();
    _avgTime = _totalDeltasPerSecond / _frames;
    _frameRate = _frames;
    _frames = 0;
    _totalDeltasPerSecond = 0.f;
  }
}

void Engine::__CreateCameraUBO(i32 bindingPoint)
{
  // Reserve memory for:
  // - 2 mat4f: camera projection + camera view
  // - 1 vec3f: camera position
  // - 1 float: padding
  constexpr auto size = sizeof(Mat4F)*2 + sizeof(Vec3F) + sizeof(f32); // bytes
  _uboCameraBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr auto zeros = Array<byte, size>{};
  _uboCameraBlock.UpdateStorage(0, size, zeros.data());
  _uboCameraBlock.BindBase(BufferTarget::UNIFORM, bindingPoint);
}
void Engine::__CreateLightUBO(i32 bindingPoint)
{
  // Reserve memory for:
  // - 1 DirectionalLight object
  // - 1 PointLight object
  // - 1 SpotLight object
  constexpr auto size = sizeof(DirectionalLight) + sizeof(PointLight) + sizeof(SpotLight);
  _uboLightBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr auto zeros = Array<byte, size>{};
  _uboLightBlock.UpdateStorage(0, size, zeros.data());
  _uboLightBlock.BindBase(BufferTarget::UNIFORM, bindingPoint);
}

Mesh Engine::__CreateMeshQuad()
{
  constexpr auto vertex1 = Vertex<Position, TextureCoord>{ Vec3F(-1.0f,  1.0f, 0.0f), Vec2F(0.0f, 1.0f) };
  constexpr auto vertex2 = Vertex<Position, TextureCoord>{ Vec3F(-1.0f, -1.0f, 0.0f), Vec2F(0.0f, 0.0f) };
  constexpr auto vertex3 = Vertex<Position, TextureCoord>{ Vec3F(1.0f,  1.0f, 0.0f),  Vec2F(1.0f, 1.0f) };
  constexpr auto vertex4 = Vertex<Position, TextureCoord>{ Vec3F(1.0f, -1.0f, 0.0f),  Vec2F(1.0f, 0.0f) };
  constexpr auto vertices = Array<Vertex<Position, TextureCoord>, 4>{ vertex1,vertex2,vertex3,vertex4, };

  constexpr auto stride = sizeof(vertex1);
  constexpr auto offsetPosition = 0;
  constexpr auto offsetTC = offsetPosition + sizeof(Position);
  constexpr auto vertexFormat0 = VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offsetPosition) };
  constexpr auto vertexFormat1 = VertexFormat{ 2, VertexAttribType::FLOAT, false, static_cast<i32>(offsetTC) };

  auto vbo = Buffer(sizeof(vertices), vertices.data(), BufferUsage::STATIC_DRAW);
  auto quad = Mesh{};
  quad.Create();
  quad.SetupAttributeFloat(0, 0, vertexFormat0);
  quad.SetupAttributeFloat(1, 0, vertexFormat1);
  quad.vertexArray->AttachVertexBuffer(0, vbo, 0, stride);
  return quad;
}
void Engine::__CreateMSAAFramebuffer(i32 w, i32 h, i32 samples)
{
  if (_MSAAFramebuffer.IsValid())
    CONSOLE_WARN("Warning in __CreateMSAAFramebuffer: _MSAAFramebuffer.IsValid()");
  if (_MSAAFramebufferResolver.IsValid())
    CONSOLE_WARN("Warning in __CreateMSAAFramebuffer: _MSAAFramebufferResolver.IsValid()");

  // 1. Create the multisampled framebuffer 
  auto msTextureColor = Texture2D{};
  msTextureColor.Create(Texture2DTarget::TEXTURE_2D_MULTISAMPLE);
  msTextureColor.CreateStorageMultisampled(Texture2DInternalFormat::RGB8, samples, w, h);
  auto msDepthStencil = Renderbuffer{};
  msDepthStencil.Create();
  msDepthStencil.CreateStorageMulstisampled(RenderbufferInternalFormat::DEPTH24_STENCIL8, samples, w, h);
  _MSAAFramebuffer = Framebuffer{};
  _MSAAFramebuffer.Create();
  _MSAAFramebuffer.AttachTexture(FramebufferAttachment::COLOR_ATTACHMENT0, msTextureColor, 0);
  _MSAAFramebuffer.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, msDepthStencil);
  assert(_MSAAFramebuffer.CheckStatus() == FramebufferStatus::COMPLETE);
  
  // 2. Create the normal framebuffer 
  _MSAAFramebufferResolver = Framebuffer{};
  _MSAAFramebufferResolver.Create();
  auto textureColor = Texture2D{};
  textureColor.Create(Texture2DTarget::TEXTURE_2D);
  textureColor.CreateStorage(Texture2DInternalFormat::RGB8, w, h);
  textureColor.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  textureColor.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  auto depthStencil = Renderbuffer{};
  depthStencil.Create();
  depthStencil.CreateStorage(RenderbufferInternalFormat::DEPTH24_STENCIL8, w, h);
  _MSAAFramebufferResolver.AttachTexture(FramebufferAttachment::COLOR_ATTACHMENT0, textureColor, 0);
  _MSAAFramebufferResolver.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, depthStencil);
  assert(_MSAAFramebufferResolver.CheckStatus() == FramebufferStatus::COMPLETE);
}
void Engine::__ResolveMSAAFramebuffer(i32 w, i32 h)
{
  _MSAAFramebuffer.Bind(FramebufferTarget::READ);
  _MSAAFramebufferResolver.Bind(FramebufferTarget::DRAW);

  // Copy the color buffer
  _MSAAFramebuffer.Blit(_MSAAFramebufferResolver,
                        0, 0, w, h,     // source rect
                        0, 0, w, h,     // dest rect
                        FramebufferBlitMask::COLOR_BUFFER,
                        FramebufferBlitFilter::LINEAR);

  // Copy the depth/stencil buffer
  _MSAAFramebuffer.Blit(_MSAAFramebufferResolver,
                        0, 0, w, h,     // source rect
                        0, 0, w, h,     // dest rect
                        FramebufferBlitMask::DEPTH_STENCIL_BUFFER,
                        FramebufferBlitFilter::NEAREST);  // NEAREST per depth/stencil

  _MSAAFramebufferResolver.Unbind(FramebufferTarget::DRAW);
  _MSAAFramebuffer.Unbind(FramebufferTarget::READ);
}
