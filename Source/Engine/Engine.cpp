#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Uniforms.hpp"
#include "Engine/Vertex.hpp"

#include "Engine/Components/Components.hpp"

#include "Engine/Graphics/DepthTest.hpp"
#include "Engine/Graphics/StencilTest.hpp"
#include "Engine/Graphics/FaceCulling.hpp"
#include "Engine/Graphics/Objects/Renderbuffer.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Managers/WindowManager.hpp"
#include "Engine/Managers/ShadersManager.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include "GUI/ImGuiLayer.hpp"

static constexpr i32 INITIAL_WINDOW_W = 1600;
static constexpr i32 INITIAL_WINDOW_H = 900;
static Vec2I s_viewportSize{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };
static f32 s_aspect = static_cast<f32>(s_viewportSize.x) / static_cast<f32>(s_viewportSize.y);
static f64 s_worldTime;

static Framebuffer s_framebufferPhong;
static Mesh s_screenQuad;

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

static Mesh CreateMeshQuad()
{
  constexpr Vertex_P_UV vertices[] = {
    Vertex_P_UV{ Vec3F(-1.0f,  1.0f, 0.0f), Vec2F(0.0f, 1.0f) },
    Vertex_P_UV{ Vec3F(-1.0f, -1.0f, 0.0f), Vec2F(0.0f, 0.0f) },
    Vertex_P_UV{ Vec3F(1.0f,  1.0f, 0.0f),  Vec2F(1.0f, 1.0f) },
    Vertex_P_UV{ Vec3F(1.0f, -1.0f, 0.0f),  Vec2F(1.0f, 0.0f) },
  };
  Buffer vbo(sizeof(vertices), vertices, BufferUsage::STATIC_DRAW);

  Mesh quad;
  quad.Create();
  quad.SetupAttributeFloat(0, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_UV, position)));
  quad.SetupAttributeFloat(1, 0, VertexFormat(2, VertexAttribType::FLOAT, false, offsetof(Vertex_P_UV, uv)));
  quad.vertexArray->AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_UV));
  return quad;
}
static Framebuffer CreateFramebufferPhong(i32 w, i32 h)
{
  // https://en.wikipedia.org/wiki/Cel_shading#Edge-detection_method
  Texture2D textureColor;
  textureColor.Create(Texture2DTarget::TEXTURE_2D);
  textureColor.CreateStorage(Texture2DInternalFormat::RGB8, w, h);
  textureColor.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  textureColor.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  
  Renderbuffer depthStencil;
  depthStencil.Create();
  depthStencil.CreateStorage(RenderbufferInternalFormat::DEPTH24_STENCIL8, w, h);

  Framebuffer framebuffer;
  framebuffer.Create();
  framebuffer.AttachTexture(FramebufferAttachment::COLOR_ATTACHMENT0, textureColor, 0);
  framebuffer.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, depthStencil);

  assert(framebuffer.CheckStatus() == FramebufferStatus::COMPLETE);
  return framebuffer;
}
static Texture2D BlinnPhongShading(Program& program, Scene& scene)
{
  s_framebufferPhong.Bind(FramebufferTarget::DRAW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers

  program.Use();
  scene.Reg().view<StaticMesh, Transformation>().each([&](auto& staticMesh, auto& transform) {
    transform.eulerAngles.y = s_worldTime * 10.f;
    program.SetUniformMat4f(UniformLocations::MODEL, transform.GetTransformation());
    staticMesh.Render(program, RenderMode::TRIANGLES);
  });

  s_framebufferPhong.Unbind(FramebufferTarget::DRAW);
  return s_framebufferPhong.GetTextureAttachment(0); // returns the texture color 
}

// -----------------------------------------------------
//                PUBLIC METHODS
// -----------------------------------------------------

void Engine::Initialize()
{
  // Initialize logger
  // -----------------
  Logger::Initialize();

  // Initialize window manager
  // -------------------------
  CONSOLE_INFO("Initializing WindowManager...");
  WindowManager::GetInstance().Initialize(WindowProps(
    Vec2I{ INITIAL_WINDOW_W, INITIAL_WINDOW_H }, // window size
    Vec2I{ 50, 50 },                      // window pos
    "GameEngine",                         // window title
    Vec2I{ 16, 9 },                       // window aspect ratio
    false                                 // window v-sync
  ));

  // Initialize shader manager
  // -------------------------
  CONSOLE_INFO("Initializing ShadersManager...");
  ShadersManager::GetInstance().Initialize();

  // Initialize texture manager
  // --------------------------
  CONSOLE_INFO("Initializing TexturesManager...");
  TexturesManager::GetInstance().Initialize();

  // Setup ImGui context
  // -------------------
  CONSOLE_INFO("Initializing ImGui...");
  ImGuiLayer::GetInstance().InitializeImGui();

  // Set the initial OpenGL states
  // -----------------------------
  SetGLStates();

  // Create UBO objects
  // -----------------------------
  CreateCameraUBO(0); // "CameraBlock" -> 0
  CreateLightUBO(1);  // "LightBlock" -> 1
  CreateBoneUBO(2);   // "BonesBlock" -> 2

  // Initialize time
  // -----------------------------
  InitTime();
}
void Engine::Run()
{
  s_screenQuad = CreateMeshQuad();
  s_framebufferPhong = CreateFramebufferPhong(INITIAL_WINDOW_W, INITIAL_WINDOW_H);

  Camera camera;
  camera.position = Vec3F(0.f, 0.f, 10.0f);

  Scene scene((Paths::GetRootPath() / "Scene.yaml"));

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  ImGuiLayer& gui = ImGuiLayer::GetInstance();
  WindowManager& windowManager = WindowManager::GetInstance();
  ShadersManager& shadersManager = ShadersManager::GetInstance();
  Program blinnPhongProgram = shadersManager.GetProgram("BlinnPhong");
  
  // ------------------------------------------------------------------
  // -------------------------- loop section --------------------------
  // ------------------------------------------------------------------
  while (windowManager.IsOpen())
  {
    gui.PrepareImGuiFrame();

    // ----------------------------------------------------------------------------------
    // -------------------------- Per-frame time logic section --------------------------
    // ----------------------------------------------------------------------------------
    CalculatePerFrameTime();
    g_drawCalls = 0;
    s_worldTime = windowManager.GetWorldTime();

    // -------------------------------------------------------------------
    // -------------------------- Input section --------------------------
    // -------------------------------------------------------------------
    windowManager.PoolEvents();
    if (gui.viewportFocused)
    {
      camera.ProcessKeyboard(static_cast<f32>(_delta), 10.0f);
      camera.ProcessMouse(static_cast<f32>(_delta), 20.0f);
    }

    // --------------------------------------------------------------------
    // -------------------------- Update section --------------------------
    // --------------------------------------------------------------------
    Mat4f cameraView = camera.GetViewMatrix();
    Mat4f cameraProj = camera.GetPerspectiveProjection(glm::radians(45.0f), s_aspect, 0.1f, 50.f);

    // Update camera UBO
    {
      auto matrices = Array<Mat4f, 2>{ cameraView, cameraProj };
      const Vec3F& camPos = camera.position;
      _uboCameraBlock.UpdateStorage(0, sizeof(matrices), matrices.data());
      _uboCameraBlock.UpdateStorage(sizeof(matrices), sizeof(Vec3F), &camPos);
    }

    // Update light UBO
    {
      constexpr u32 size = sizeof(DirectionalLight) + sizeof(PointLight);
      constexpr Array<u8, size> zeros{};
      _uboLightBlock.UpdateStorage(0, size, zeros.data());

      auto e = scene.FindObjectWithComponent<DirectionalLight>();
      if (e.has_value())
      {
        GameObject obj = e.value();
        DirectionalLight* light = obj.GetComponent<DirectionalLight>();
        _uboLightBlock.UpdateStorage(0,
          sizeof(DirectionalLight),
          reinterpret_cast<void*>(light));
      }
      e = scene.FindObjectWithComponent<PointLight>();
      if (e.has_value())
      {
        GameObject obj = e.value();
        PointLight* light = obj.GetComponent<PointLight>();
        _uboLightBlock.UpdateStorage(sizeof(DirectionalLight),
          sizeof(PointLight),
          reinterpret_cast<void*>(light));
      }
    }

    // -----------------------------------------------------------------------
    // -------------------------- Rendering section --------------------------
    // -----------------------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, g_renderInWireframe ? GL_LINE : GL_FILL);
    glViewport(0, 0, s_viewportSize.x, s_viewportSize.y);

    blinnPhongProgram.SetUniform1i("u_normalMapping", static_cast<i32>(g_renderWithNormalMapping));
    Texture2D viewportImage = BlinnPhongShading(blinnPhongProgram, scene);

    gui.MenuBar(scene);
    gui.ImguiDemo();
    GameObject& objSelected = gui.Hierarchy(scene);
    gui.Inspector(objSelected);
    gui.Viewport(viewportImage, objSelected, cameraView, cameraProj);
    gui.ToolBar();
    gui.TimeInfo(_delta, _avgTime, _frameRate);
    gui.CameraProperties(camera);
    gui.GraphicsInfo();
    
    gui.CompleteFrameRender();

    // Checking viewport size
    if (s_viewportSize != gui.viewportSize)
    {
      s_viewportSize = gui.viewportSize;
      s_aspect = static_cast<f32>(s_viewportSize.x) / static_cast<f32>(s_viewportSize.y);
      s_framebufferPhong.Delete();
      s_framebufferPhong = CreateFramebufferPhong(s_viewportSize.x, s_viewportSize.y);
    }

    // ------------------------------------------------------------------
    // -------------------------- Swap buffers --------------------------
    // ------------------------------------------------------------------
    windowManager.SwapWindowBuffers();
  }
}
void Engine::CleanUp()
{
  _uboCameraBlock.Delete();
  _uboLightBlock.Delete();
  _uboBoneBlock.Delete();

  ImGuiLayer::GetInstance().CleanUpImGui();
  ShadersManager::GetInstance().CleanUp();
  TexturesManager::GetInstance().CleanUp();
  WindowManager::GetInstance().CleanUp();
}

// -----------------------------------------------------
//            PRIVATE METHODS
// -----------------------------------------------------

void Engine::SetGLStates() const
{
  // Enable debug output
  // -------------------
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

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

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // specify clear values for the color buffers
  glClearDepth(1.0f);                       // specify clear values for the depth buffer    
  glClearStencil(0);                        // specify clear values for the stencil buffer 
}
void Engine::InitTime()
{
  _now = chrono::steady_clock::time_point{};
  _lastFrameTime = chrono::steady_clock::time_point{};
  _timerT0 = chrono::steady_clock::now();
  _timerT1 = chrono::steady_clock::time_point{};
  _frames = 0;
  _frameRate = 0;
  _totalDeltasPerSecond = 0.0f;
  _avgTime = 0.0f;
  _delta = 0.0f;
}
void Engine::CalculatePerFrameTime()
{
  _frames++;

  _now = chrono::steady_clock::now();
  _delta = chrono::duration_cast<chrono::duration<f64>>(_now - _lastFrameTime).count();
  _lastFrameTime = _now;
  _totalDeltasPerSecond += _delta;
  _timerT1 = chrono::steady_clock::now();
  f64 timer_diff = chrono::duration_cast<chrono::duration<f64>>(_timerT1 - _timerT0).count();
  if (timer_diff >= 1.f)
  {
    _timerT0 = chrono::steady_clock::now();
    _avgTime = _totalDeltasPerSecond / _frames;
    _frameRate = _frames;
    _frames = 0;
    _totalDeltasPerSecond = 0.f;
  }
}

void Engine::CreateCameraUBO(i32 bindingPoint)
{
  // Reserve memory for:
  // - 2 mat4f: camera projection + camera view
  // - 1 vec3f: camera position
  // - 1 float: padding
  constexpr u32 size = 2 * sizeof(Mat4f) +
    sizeof(Vec3F) +
    sizeof(f32);

  _uboCameraBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboCameraBlock.UpdateStorage(0, size, zeros.data()); // Init buffer with zeros
  _uboCameraBlock.BindBase(BufferTarget::UNIFORM, bindingPoint);   // CameraBlock
}
void Engine::CreateLightUBO(i32 bindingPoint)
{
  // Reserve memory for:
  // - 1 DirectionalLight object
  // - 1 PointLight object
  constexpr u32 size = sizeof(DirectionalLight) + sizeof(PointLight);
  _uboLightBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboLightBlock.UpdateStorage(0, size, zeros.data());
  _uboLightBlock.BindBase(BufferTarget::UNIFORM, bindingPoint);
}
void Engine::CreateBoneUBO(i32 bindingPoint)
{
  constexpr u32 size = SkeletalMesh::GetMaxNumBones() * sizeof(Mat4f);
  _uboBoneBlock = Buffer(size, nullptr, BufferUsage::STREAM_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboBoneBlock.UpdateStorage(0, size, zeros.data());
  _uboBoneBlock.BindBase(BufferTarget::UNIFORM, bindingPoint);
}
