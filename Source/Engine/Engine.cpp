#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Ext.hpp"
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
#include "Engine/Graphics/Objects/RenderBuffer.hpp"
#include "Engine/Graphics/Objects/TextureCubemap.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Managers/WindowManager.hpp"
#include "Engine/Managers/ShadersManager.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/StaticMeshFactory.hpp"
#include "Engine/Managers/AnimationsManager.hpp"

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


static void CreateCube(Mesh& cube)
{
  constexpr f32 cubeVertices[] = {
    // x, y, z, nx, ny, nz, u, v, tx, ty, tz
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f, // V0
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f, // V1
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f, // V2
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f, // V3
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f, // V4
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f, // V5
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f, // V6
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f  // V7
  };
  constexpr u32 cubeIndices[] = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Back face
    4, 5, 6,  6, 7, 4,
    // Left face
    4, 0, 3,  3, 7, 4,
    // Right face
    1, 5, 6,  6, 2, 1,
    // Bottom face
    4, 5, 1,  1, 0, 4,
    // Top face
    3, 2, 6,  6, 7, 3
  };

  cube.Create();
  cube.SetupAttributeFloat(0, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, position)));
  cube.SetupAttributeFloat(1, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, normal)));
  cube.SetupAttributeFloat(2, 0, VertexFormat(2, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, uv)));
  cube.SetupAttributeFloat(3, 0, VertexFormat(3, VertexAttribType::FLOAT, false, offsetof(Vertex_P_N_UV_T, tangent)));

  Buffer vbo(sizeof(cubeVertices), cubeVertices, BufferUsage::STATIC_DRAW);
  Buffer ebo(sizeof(cubeIndices), cubeIndices, BufferUsage::STATIC_DRAW);
  cube.vertexArray->AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_N_UV_T));
  cube.vertexArray->AttachElementBuffer(ebo);

  cube.numVertices = 8;
  cube.numIndices = 36;
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
    vec2i{ WINDOW_WIDTH, WINDOW_HEIGHT }, // window size
    vec2i{ 50, 50 },                      // window pos
    "GameEngine",                       // window title
    vec2i{ 16, 9 },                       // window aspect ratio
    false                               // window v-sync
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
  ImGuiLayer::GetInstance().Initialize();

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
  camera.position = vec3f(0.f, 5.f, 10.0f);
  camera.orientation = vec3f(-90.f, -20.f, 0.0f);
  camera.frustum.zFar = 50.0f;

  Scene scene((Paths::GetRootPath() / "Scene.yaml"));

  bool wireframe = false;
  bool normalMapping = true;

  // 0: Blinn-Phong shading
  // 1: Gooch shading
  u32 shadingModel = 0;
  // gooch shading attributes:
  f32 b = 0.4f, y = 0.4f, alpha = 0.2f, beta = 0.6f;

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  ImGuiLayer& gui = ImGuiLayer::GetInstance();
  WindowManager& windowManager = WindowManager::GetInstance();
  ShadersManager& shadersManager = ShadersManager::GetInstance();
  TexturesManager& texturesManager = TexturesManager::GetInstance();

  Program blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);

  Program goochProgram = shadersManager.GetProgram("GoochShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);

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

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    // Fill the framebuffer color texture
    _fboMultisampled.Bind(FramebufferTarget::READ_DRAW);
    {
      glViewport(0, 0, _viewportSize.x, _viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // Render scene here
      // ---------------------
      {
        switch (shadingModel)
        {
          
          case 0: // Blinn-Phong shading model
            blinnPhongProgram.Use();
            blinnPhongProgram.SetUniform1i("u_normalMapping", normalMapping ? 1 : 0);
            scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
              transform.rotation.y = glfwGetTime() * 10.f;
              transform.UpdateTransformation();

              blinnPhongProgram.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
              staticMesh.Render(blinnPhongProgram, RenderMode::TRIANGLES); 
            });
            break;

          case 1: // Gooch shading model
            goochProgram.Use();
            goochProgram.SetUniform1f("u_b", b);
            goochProgram.SetUniform1f("u_y", y);
            goochProgram.SetUniform1f("u_alpha", alpha);
            goochProgram.SetUniform1f("u_beta", beta);
            scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
              goochProgram.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
              staticMesh.Render(goochProgram, RenderMode::TRIANGLES);
            });
            break;
        }
      }
    }
    // Blit multisampled buffer to normal color buffer of intermediate FBO
    _fboMultisampled.Blit(_fboIntermediate,
                          0, 0, _viewportSize.x, _viewportSize.y,
                          0, 0, _viewportSize.x, _viewportSize.y,
                          FramebufferBlitMask::COLOR_BUFFER,
                          FramebufferBlitFilter::NEAREST);
    _fboMultisampled.Unbind(FramebufferTarget::READ_DRAW);

    gui.RenderMenuBar(scene);
    GameObject& objSelected = gui.RenderHierarchy(scene);
    gui.RenderInspector(objSelected);
    u32 fboTexture = _fboIntermediate.textAttachments.at(0);
    gui.RenderViewport(fboTexture, objSelected, cameraView, cameraProj);
    gui.RenderTimeInfo(_delta, _avgTime, _frameRate);
    gui.RenderGizmoToolBar();
    gui.RenderCameraSettings(camera);
    gui.RenderDebug(wireframe, normalMapping, shadingModel, b, y, alpha, beta);
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
  ImGuiLayer::GetInstance().CleanUp();

  // clean up all managers
  ShadersManager::GetInstance().CleanUp();
  TexturesManager::GetInstance().CleanUp();
  WindowManager::GetInstance().CleanUp(); // !!Raise exception here
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

  _screenSquare.vertexArray->AttachVertexBuffer(0, vbo, 0, 4 * sizeof(f32));
  _screenSquare.vertexArray->SetAttribFormatFLoat(0, 2, VertexAttribType::FLOAT, true, 0);
  _screenSquare.vertexArray->SetAttribBinding(0, 0);
  _screenSquare.vertexArray->EnableAttribute(0);
  _screenSquare.vertexArray->SetAttribFormatFLoat(1, 2, VertexAttribType::FLOAT, true, 2 * sizeof(f32));
  _screenSquare.vertexArray->SetAttribBinding(1, 0);
  _screenSquare.vertexArray->EnableAttribute(1);
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
