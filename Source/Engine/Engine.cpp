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
#include "Engine/Graphics/Objects/TextureCubemap.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Managers/WindowManager.hpp"
#include "Engine/Managers/ShadersManager.hpp"
#include "Engine/Managers/TexturesManager.hpp"

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

static constexpr i32 INITIAL_WINDOW_W = 1600;
static constexpr i32 INITIAL_WINDOW_H = 900;

static vec2i s_viewportSize{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };

struct GoochParams
{
  f32 warmFactor = 0.25f;    // alpha: warm contribution
  f32 coolFactor = 0.50f;    // beta: cool contribution
  f32 baseCoolColor = 0.55f; // b: cool color base
  f32 baseWarmColor = 0.3f;  // y: warm color base
};
static GoochParams s_goochParams;

static Framebuffer s_depthMapFramebuffer;
static Framebuffer s_blinnPhongFramebuffer;
static Framebuffer s_goochFramebuffer;
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

static Framebuffer CreateDepthMapFramebuffer(i32 w, i32 h)
{
  Texture2D depthTextureRGB;
  depthTextureRGB.Create(Texture2DTarget::TEXTURE_2D);
  depthTextureRGB.CreateStorage(Texture2DInternalFormat::RGB8, w, h);
  depthTextureRGB.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::NEAREST);
  depthTextureRGB.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::NEAREST);

  Renderbuffer depth;
  depth.Create();
  depth.CreateStorage(RenderbufferInternalFormat::DEPTH_COMPONENT24, w, h);

  Framebuffer framebuffer;
  framebuffer.Create();
  framebuffer.AttachTexture(FramebufferAttachment::COLOR_0, depthTextureRGB, 0);
  framebuffer.AttachRenderBuffer(FramebufferAttachment::DEPTH, depth);
  assert(framebuffer.CheckStatus() == FramebufferStatus::COMPLETE);
  return framebuffer;
}
static Framebuffer CreateBlinnPhongFramebuffer(i32 w, i32 h)
{
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
	framebuffer.AttachTexture(FramebufferAttachment::COLOR_0, textureColor, 0);
  framebuffer.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, depthStencil);
  assert(framebuffer.CheckStatus() == FramebufferStatus::COMPLETE);
  return framebuffer;
}
static Framebuffer CreateGoochFramebuffer(i32 w, i32 h)
{
  // https://en.wikipedia.org/wiki/Cel_shading#Edge-detection_method
  // 1. texture color: the scene is rendered with cel shading to a screen-sized color texture.
  // 2. depth/stencil: depth information of the scene is rendered to a screen-sized texture
  // 3. normals: World-space surface normals are rendered as a screen-sized texture

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
  framebuffer.AttachTexture(FramebufferAttachment::COLOR_0, textureColor, 0);
  framebuffer.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, depthStencil);
  assert(framebuffer.CheckStatus() == FramebufferStatus::COMPLETE);
  return framebuffer;
}

static Texture2D BlinnPhongShading(Program& program, Scene& scene)
{
  s_blinnPhongFramebuffer.Bind(FramebufferTarget::DRAW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers

  program.Use();
  scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
    transform.rotation.y = glfwGetTime() * 10.f;
    transform.UpdateTransformation();
    program.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
    staticMesh.Render(program, RenderMode::TRIANGLES);
  });

  s_blinnPhongFramebuffer.Unbind(FramebufferTarget::DRAW);
  return s_blinnPhongFramebuffer.GetTextureAttachment(0); // returns the texture color 
}
static Texture2D GoochShading(Program& program, Scene& scene)
{
  s_goochFramebuffer.Bind(FramebufferTarget::DRAW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers

  program.Use();
  scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
    transform.rotation.y = glfwGetTime() * 10.f;
    transform.UpdateTransformation();
    program.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
    staticMesh.Render(program, RenderMode::TRIANGLES);
  });

  s_goochFramebuffer.Unbind(FramebufferTarget::DRAW);
  return s_goochFramebuffer.GetTextureAttachment(0); // returns the texture color
}
static Texture2D DepthMapRendering(Program& program, Scene& scene)
{
  s_depthMapFramebuffer.Bind(FramebufferTarget::DRAW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear buffers
  
  program.Use();
  scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
    transform.rotation.y = glfwGetTime() * 10.f;
    transform.UpdateTransformation();
    program.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
    for (u32 i = 0; i < staticMesh.nrMeshes; i++)
    {
      auto& mesh = staticMesh.meshArray[i];
      Renderer::DrawElements(RenderMode::TRIANGLES, *mesh.vertexArray, mesh.numIndices);
    }
  });
  s_depthMapFramebuffer.Unbind(FramebufferTarget::DRAW);
  return s_depthMapFramebuffer.GetTextureAttachment(0); // returns the depth in rgb
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
    vec2i{ INITIAL_WINDOW_W, INITIAL_WINDOW_H }, // window size
    vec2i{ 50, 50 },                      // window pos
    "GameEngine",                         // window title
    vec2i{ 16, 9 },                       // window aspect ratio
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
  CreateCameraUBO();  // "CameraBlock"
  CreateLightUBO();   // "LightBlock"
  CreateBoneUBO();    // "BoneBlock"

  // Initialize time
  // -----------------------------
  InitTime();
}
void Engine::Run()
{
  s_depthMapFramebuffer = CreateDepthMapFramebuffer(INITIAL_WINDOW_W, INITIAL_WINDOW_H);
  s_blinnPhongFramebuffer = CreateBlinnPhongFramebuffer(INITIAL_WINDOW_W, INITIAL_WINDOW_H);
  s_goochFramebuffer = CreateGoochFramebuffer(INITIAL_WINDOW_W, INITIAL_WINDOW_H);

  Camera camera;
  camera.position = vec3f(0.f, 5.f, 10.0f);
  camera.orientation = vec3f(-90.f, -20.f, 0.0f);
  camera.frustum.zFar = 50.0f;

  Scene scene((Paths::GetRootPath() / "Scene.yaml"));

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  ImGuiLayer& gui = ImGuiLayer::GetInstance();
  WindowManager& windowManager = WindowManager::GetInstance();
  ShadersManager& shadersManager = ShadersManager::GetInstance();
  TexturesManager& texturesManager = TexturesManager::GetInstance();
  Program depthMapProgram = shadersManager.GetProgram("DepthMap");
  Program blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);
  Program goochProgram = shadersManager.GetProgram("GoochShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);

  Texture2D viewportImage;

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
    camera.UpdateOrientation();
    mat4f cameraView = camera.CalculateView(camera.position + camera.GetFrontVector());
    mat4f cameraProj = camera.CalculatePerspective(static_cast<f32>(s_viewportSize.x) / s_viewportSize.y);

    // Update camera UBO
    {
      auto matrices = Array<mat4f, 2>{ cameraView, cameraProj };
      const vec3f& camPos = camera.position;
      _uboCameraBlock.UpdateStorage(0, sizeof(matrices), matrices.data());
      _uboCameraBlock.UpdateStorage(sizeof(matrices), sizeof(vec3f), &camPos);
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
    
    switch (g_renderMode)
    {
      case 0: // render color
      { 
        if (g_activeShadingModel == 0) // Render with Blinn-Phong shading model
        {
          blinnPhongProgram.SetUniform1i("u_normalMapping", g_renderWithNormalMapping ? 1 : 0);
          viewportImage = BlinnPhongShading(blinnPhongProgram, scene);
        }
        else if (g_activeShadingModel == 1) // Render with Gooch shading 
        {
          goochProgram.SetUniform1i("u_normalMapping", g_renderWithNormalMapping ? 1 : 0);
          goochProgram.SetUniform1f("u_goochParams.baseCoolColor", s_goochParams.baseCoolColor);
          goochProgram.SetUniform1f("u_goochParams.baseWarmColor", s_goochParams.baseWarmColor);
          goochProgram.SetUniform1f("u_goochParams.coolFactor", s_goochParams.coolFactor);
          goochProgram.SetUniform1f("u_goochParams.warmFactor", s_goochParams.warmFactor);
          viewportImage = GoochShading(blinnPhongProgram, scene);
        }
        break;
      }

      case 1: // render the depth map
      {
        depthMapProgram.SetUniform1f("u_zNear", camera.frustum.zNear);
        depthMapProgram.SetUniform1f("u_zFar", camera.frustum.zFar);
        viewportImage = DepthMapRendering(depthMapProgram, scene);
        break;
      }

      case 2: // render normals
      {
        break;
      }
    }
    

    gui.MenuBar(scene, s_goochParams);
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
      
      s_depthMapFramebuffer.Delete();
      s_depthMapFramebuffer = CreateDepthMapFramebuffer(s_viewportSize.x, s_viewportSize.y);
      s_blinnPhongFramebuffer.Delete();
      s_blinnPhongFramebuffer = CreateBlinnPhongFramebuffer(s_viewportSize.x, s_viewportSize.y);
      s_goochFramebuffer.Delete();
      s_goochFramebuffer = CreateGoochFramebuffer(s_viewportSize.x, s_viewportSize.y);
    }

    // ------------------------------------------------------------------
    // -------------------------- Swap buffers --------------------------
    // ------------------------------------------------------------------
    windowManager.SwapWindowBuffers();
  }

  s_depthMapFramebuffer.Delete();
	s_goochFramebuffer.Delete();
	s_blinnPhongFramebuffer.Delete();
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

void Engine::CreateCameraUBO()
{
  // Reserve memory for:
  // - 2 mat4f: camera projection + camera view
  // - 1 vec3f: camera position
  // - 1 float: padding
  constexpr u32 size = 2 * sizeof(mat4f) +
    sizeof(vec3f) +
    sizeof(f32);

  _uboCameraBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboCameraBlock.UpdateStorage(0, size, zeros.data()); // Init buffer with zeros
  _uboCameraBlock.BindBase(BufferTarget::UNIFORM, 0);
}
void Engine::CreateLightUBO()
{
  // Reserve memory for:
  // - 1 DirectionalLight object
  // - 1 PointLight object
  constexpr u32 size = sizeof(DirectionalLight) + sizeof(PointLight);
  _uboLightBlock = Buffer(size, nullptr, BufferUsage::DYNAMIC_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboLightBlock.UpdateStorage(0, size, zeros.data()); // Init buffer with zeros
  _uboLightBlock.BindBase(BufferTarget::UNIFORM, 1);
}
void Engine::CreateBoneUBO()
{
  constexpr u32 size = SkeletalMesh::GetMaxNumBones() * sizeof(mat4f);
  _uboBoneBlock = Buffer(size, nullptr, BufferUsage::STREAM_DRAW);

  constexpr Array<u8, size> zeros{};
  _uboBoneBlock.UpdateStorage(0, size, zeros.data()); // Init buffer with zeros
  _uboBoneBlock.BindBase(BufferTarget::UNIFORM, 2);
}
