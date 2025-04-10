#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Ext.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Uniforms.hpp"
#include "Engine/ImageLoader.hpp"

#include "Engine/ECS/ECS.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/DepthTest.hpp"
#include "Engine/Graphics/StencilTest.hpp"
#include "Engine/Graphics/FaceCulling.hpp"
#include "Engine/Graphics/Objects/RenderBuffer.hpp"
#include "Engine/Graphics/Objects/TextureCubemap.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShadersManager.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Subsystems/AnimationsManager.hpp"

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

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
  // Initialize logger
  // -----------------
  Logger::Initialize();

  // Initialize window manager
  // -------------------------
  CONSOLE_INFO("Initializing WindowManager...");
  WindowManager::Get().Initialize(WindowProps(
    vec2i{ WINDOW_WIDTH, WINDOW_HEIGHT }, // window size
    vec2i{ 50, 50 },                      // window pos
    "GameEngine",                       // window title
    vec2i{ 16, 9 },                       // window aspect ratio
    false                               // window v-sync
  ));

  // Initialize shader manager
  // -------------------------
  CONSOLE_INFO("Initializing ShadersManager...");
  ShadersManager::Get().Initialize();

  // Initialize texture manager
  // --------------------------
  CONSOLE_INFO("Initializing TexturesManager...");
  TexturesManager::Get().Initialize();

  // Setup ImGui context
  // -------------------
  CONSOLE_INFO("Initializing ImGui...");
  ImGuiLayer::Get().Initialize();

  // Set the initial OpenGL states
  // -----------------------------
  SetGLStates();

  // Create UBO objects
  // -----------------------------
  CreateCameraUBO();  // "CameraBlock"
  CreateLightUBO();   // "LightBlock"
  CreateBoneUBO();    // "BoneBlock"

  // Create framebuffer
  // -----------------------------
  CreateFramebuffer(4, WINDOW_WIDTH, WINDOW_HEIGHT);
  CreateScreenSquare();

  // Initialize time
  // -----------------------------
  InitTime();
}
void Engine::Run()
{
  Camera camera;
  camera.position = vec3f(0.f, 15.f, 30.0f);
  camera.orientation = vec3f(-90.f, -35.f, 0.0f);
  camera.frustum.zFar = 50.0f;

  Scene scene((Paths::GetRootPath() / "Scene.yaml"));

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  ImGuiLayer& gui = ImGuiLayer::Get();
  WindowManager& windowManager = WindowManager::Get();
  ShadersManager& shadersManager = ShadersManager::Get();
  TexturesManager& texturesManager = TexturesManager::Get();

  Program skyboxProgram = shadersManager.GetProgram("Skybox");
  skyboxProgram.SetUniform1i(Uniforms::skyboxTexture, 0);
  Program blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);
  Program goochProgram = shadersManager.GetProgram("GoochShading");
  //goochProgram.SetUniform1i("u_material.diffuseTexture", 0);
  //goochProgram.SetUniform1i("u_material.specularTexture", 1);
  //goochProgram.SetUniform1i("u_material.normalTexture", 2);

  //Array<Texture2D, 6> faces = {
  //    texturesManager.GetOrCreateTexture("skybox/right.jpg"),
  //    texturesManager.GetOrCreateTexture("skybox/left.jpg"),
  //    texturesManager.GetOrCreateTexture("skybox/top.jpg"),
  //    texturesManager.GetOrCreateTexture("skybox/bottom.jpg"),
  //    texturesManager.GetOrCreateTexture("skybox/front.jpg"),
  //    texturesManager.GetOrCreateTexture("skybox/back.jpg"),
  //};
  //TextureCubemap textureCubemap = CreateSkybox(faces);

  bool wireframe = false;
  i32 normalMapping = 0;

  // ------------------------------------------------------------------
  // -------------------------- loop section --------------------------
  // ------------------------------------------------------------------
  while (windowManager.IsOpen())
  {
    gui.BeginFrame();

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
    mat4f cameraProj = camera.CalculatePerspective(static_cast<f32>(_viewportSize.x) / static_cast<f32>(_viewportSize.y));

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

    /// Fill the framebuffer color texture
    _fboMultisampled.Bind(FramebufferTarget::READ_DRAW);
    {
      glViewport(0, 0, _viewportSize.x, _viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

      /// Render scene here
      {
        blinnPhongProgram.Use();
        blinnPhongProgram.SetUniform1i("u_normalMapping", normalMapping);
        scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform)
          {
            transform.UpdateTransformation();
            blinnPhongProgram.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
            staticMesh.Render(blinnPhongProgram, RenderMode::TRIANGLES); });
      }

      /// Draw skybox after the scene
      {
        //skyboxProgram.Use();
        //textureCubemap.BindTextureUnit(0);
        //DepthTest::SetDepthFun(CompareFunc::LEQUAL);
        //Renderer::DrawArrays(RenderMode::TRIANGLES, _meshCubeSkybox.vao, _meshCubeSkybox.numVertices);
        //DepthTest::SetDepthFun(CompareFunc::LESS);
      }

      // Blit multisampled buffer to normal color buffer of intermediate FBO
      _fboMultisampled.Blit(_fboIntermediate,
        0, 0, _viewportSize.x, _viewportSize.y,
        0, 0, _viewportSize.x, _viewportSize.y,
        FramebufferBlitMask::COLOR_BUFFER,
        FramebufferBlitFilter::NEAREST);
      }
    _fboMultisampled.Unbind(FramebufferTarget::READ_DRAW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gui.RenderMenuBar(scene);
    GameObject& objSelected = gui.RenderHierarchy(scene);
    gui.RenderInspector(objSelected);
    u32 fboTexture = _fboIntermediate.textAttachments.at(0);
    gui.RenderViewport(fboTexture, objSelected, cameraView, cameraProj);
    gui.RenderTimeInfo(_delta, _avgTime, _frameRate);
    gui.RenderGizmoToolBar();
    gui.RenderCameraSettings(camera);
    gui.RenderDebug(wireframe, normalMapping);
    gui.EndFrame();

    // Checking viewport size
    if (_viewportSize != gui.viewportSize)
    {
      _fboMultisampled.Delete();
      _fboIntermediate.Delete();
      CreateFramebuffer(4, gui.viewportSize.x, gui.viewportSize.y);
    }

    // ------------------------------------------------------------------
    // -------------------------- Swap buffers --------------------------
    // ------------------------------------------------------------------
    windowManager.SwapWindowBuffers();
  }
}
void Engine::CleanUp()
{
  // Destroy all framebuffers
  _fboIntermediate.Delete();
  _fboMultisampled.Delete();

  // Destroy all meshes
  _screenSquare.Destroy();
  _meshCubeSkybox.Destroy();

  // Destroy all uniform block objects
  _uboCameraBlock.Delete();
  _uboLightBlock.Delete();
  _uboBoneBlock.Delete();

  // Destroy ImGui context
  ImGuiLayer::Get().CleanUp();

  // clean up all managers
  ShadersManager::Get().CleanUp();
  TexturesManager::Get().CleanUp();
  ModelsManager::Get().CleanUp();
  WindowManager::Get().CleanUp(); // !!Raise exception here
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
  StencilTest::SetStencilFun(CompareFunc::ALWAYS, 0, 0xFF);
  StencilTest::SetStencilOp(StencilOpMode::KEEP, StencilOpMode::KEEP, StencilOpMode::KEEP);

  // Culling OFF
  // -----------
  FaceCulling::DisableFaceCulling();
  FaceCulling::SetCullFace(CullFaceMode::BACK);
  FaceCulling::SetFrontFacing(FrontFaceMode::CCW);

  // Blending OFF
  // ------------
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Gamma correction OFF
  // --------------------
  glDisable(GL_FRAMEBUFFER_SRGB);

  // Antialising ON
  // --------------
  glEnable(GL_MULTISAMPLE);

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);
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

void Engine::CreateFramebuffer(i32 samples, i32 width, i32 height)
{
  _viewportSize = { width, height };
  _fboMultisampled.Create();

  // Create a multisampled color attachment texture
  Texture2D textColMultAtt;
  textColMultAtt.Create(Texture2DTarget::TEXTURE_2D_MULTISAMPLE);
  textColMultAtt.CreateStorageMultisampled(Texture2DInternalFormat::RGB8, samples, width, height);
  // Create a multisampled renderbuffer object for depth and stencil attachments
  RenderBuffer depthStencMultAtt;
  depthStencMultAtt.Create();
  depthStencMultAtt.CreateStorageMulstisampled(RenderbufferInternalFormat::DEPTH24_STENCIL8, samples, width, height);
  _fboMultisampled.AttachTexture(FramebufferAttachment::COLOR_0, textColMultAtt.id, 0);
  _fboMultisampled.AttachRenderBuffer(FramebufferAttachment::DEPTH_STENCIL, depthStencMultAtt);

  if (_fboMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  // Configure second post - processing framebuffer
  _fboIntermediate.Create();

  // Create normal color attachment texture
  Texture2D textColAtt;
  textColAtt.Create(Texture2DTarget::TEXTURE_2D);
  textColAtt.CreateStorage(Texture2DInternalFormat::RGB8, width, height);
  textColAtt.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  textColAtt.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  _fboIntermediate.AttachTexture(FramebufferAttachment::COLOR_0, textColAtt.id, 0);

  if (_fboIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
}
void Engine::CreateScreenSquare()
{
  constexpr f32 vertices[] = {
    // position    uv
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f };
  Buffer vbo(sizeof(vertices), vertices, BufferUsage::STATIC_DRAW);

  _screenSquare.Create();
  _screenSquare.numVertices = 6;
  _screenSquare.numIndices = 0;

  _screenSquare.vao.AttachVertexBuffer(0, vbo, 0, 4 * sizeof(f32));
  _screenSquare.vao.SetAttribFormatFLoat(0, 2, VertexAttribType::FLOAT, true, 0);
  _screenSquare.vao.SetAttribBinding(0, 0);
  _screenSquare.vao.EnableAttribute(0);
  _screenSquare.vao.SetAttribFormatFLoat(1, 2, VertexAttribType::FLOAT, true, 2 * sizeof(f32));
  _screenSquare.vao.SetAttribBinding(1, 0);
  _screenSquare.vao.EnableAttribute(1);
}
TextureCubemap Engine::CreateSkybox(const Array<Texture2D, 6>& faces)
{
  constexpr f32 vertices[] = {
    // Position
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f };
  Buffer vbo(sizeof(vertices), vertices, BufferUsage::STATIC_DRAW);

  _meshCubeSkybox.Create();
  _meshCubeSkybox.numVertices = 36;
  _meshCubeSkybox.numIndices = 0;
  _meshCubeSkybox.vao.EnableAttribute(0);
  _meshCubeSkybox.vao.SetAttribBinding(0, 0);
  _meshCubeSkybox.vao.SetAttribFormatFLoat(0, 3, VertexAttribType::FLOAT, false, 0);
  _meshCubeSkybox.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P));

  Texture2DInternalFormat cubemapInternalFormat = faces.at(0).GetInternalFormat();
  i32 width = faces.at(0).GetWidth();
  i32 height = faces.at(0).GetHeight();

  TextureCubemap skyboxTexture;
  skyboxTexture.Create();
  skyboxTexture.CreateStorage(cubemapInternalFormat, width, height);
  skyboxTexture.LoadImages(faces);
  skyboxTexture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  skyboxTexture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_R, TextureParameteriParam::CLAMP_TO_EDGE);
  return skyboxTexture;
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
