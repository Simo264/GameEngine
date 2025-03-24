#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Ext.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Uniforms.hpp"

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

static chrono::steady_clock::time_point now{};
static chrono::steady_clock::time_point lastFrameTime{};
static f64 delta = 0.0f; // Time elapsed between two frames

static chrono::steady_clock::time_point timerT0 = chrono::steady_clock::now();
static chrono::steady_clock::time_point timerT1{};
static i32 frames = 0;
static i32 frameRate = 0; // How many frames generated per seconds
static f64 totalDeltasPerSecond = 0.0f;
static f64 avgTime = 0.0f; // The average rendering time per seconds

static void GLAPIENTRY MessageCallback(GLenum source, 
                                       GLenum type, 
                                       GLuint id, 
                                       GLenum severity, 
                                       GLsizei length, 
                                       const GLchar* message, 
                                       const void* userParam)
{
  // Ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
    return;

  const char* sourceStr;
  const char* typeStr;
  const char* severityStr;
  
  switch (source) 
  {
    case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
    default:                              sourceStr = "Unknown"; break;
  }

  switch (type) 
  {
    case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
    default:                                typeStr = "Unknown"; break;
  }

  switch (severity) 
  {
    case GL_DEBUG_SEVERITY_HIGH:         severityStr = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          severityStr = "Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; break;
    default:                             severityStr = "Unknown"; break;
  }
  CONSOLE_ERROR("GL CALLBACK: {} type = {}, severity = {}, message = {}", sourceStr, typeStr, severityStr, message);
}

static void SetOpenGLStates()
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
  FaceCulling::EnableFaceCulling();
  FaceCulling::SetCullFace(CullFaceMode::BACK);
  FaceCulling::SetFrontFacing(FrontFaceMode::CCW);

  // Blending OFF
  // ------------
  glEnable(GL_BLEND);
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
static void CalculatePerFrameTime()
{
  frames++;

  now = chrono::steady_clock::now();
  delta = chrono::duration_cast<chrono::duration<f64>>(now - lastFrameTime).count();
  lastFrameTime = now;
  totalDeltasPerSecond += delta;
  timerT1 = chrono::steady_clock::now();
  f64 timer_diff = chrono::duration_cast<chrono::duration<f64>>(timerT1 - timerT0).count();
  if (timer_diff >= 1)
  {
    timerT0 = chrono::steady_clock::now();
    avgTime = totalDeltasPerSecond / frames;
    frameRate = frames;
    frames = 0;
    totalDeltasPerSecond = 0;
  }
}
//static FrameBuffer CreateDepthMapFbo(i32 width, i32 height)
//{
//  // Create a 2D texture that we'll use as the framebuffer's depth buffer
//  Texture2D depthMap;
//  depthMap.Create(Texture2DTarget::TEXTURE_2D);
//  depthMap.CreateStorage(Texture2DInternalFormat::DEPTH_COMPONENT24, width, height);
//  depthMap.SetParameteri(TextureParameteriName::COMPARE_MODE, TextureParameteriParam::COMPARE_REF_TO_TEXTURE);
//  depthMap.SetCompareFunc(CompareFunc::LEQUAL);
//  depthMap.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
//  depthMap.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
//
//  // Resolve the problem of over sampling
//  depthMap.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::CLAMP_TO_BORDER);
//  depthMap.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::CLAMP_TO_BORDER);
//  depthMap.SetParameterfv(TextureParameteriName::BORDER_COLOR, Array<f32, 4>{ 1.0, 1.0, 1.0, 1.0 }.data());
//
//  // With the generated depth texture we can attach it as the framebuffer's depth buffer
//  FrameBuffer fbo;
//  fbo.Create();
//  fbo.AttachTexture(FramebufferAttachment::DEPTH, depthMap.id, 0);
//  return fbo;
//}
//static FrameBuffer CreateDepthCubeMapFbo(i32 width, i32 height)
//{
//  FrameBuffer fbo;
//  fbo.Create();
//
//  TextureCubemap texture;
//  texture.Create();
//  texture.CreateStorage(Texture2DInternalFormat::DEPTH_COMPONENT24, width, height);
//  for (i32 i = 0; i < 6; i++)
//    texture.SubImage3D(0, 0, 0, i, width, height, 1, Texture3DFormat::DEPTH_COMPONENT, Texture3DType::FLOAT, nullptr);
//
//  texture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
//  texture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
//  texture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::CLAMP_TO_EDGE);
//  texture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::CLAMP_TO_EDGE);
//  texture.SetParameteri(TextureParameteriName::WRAP_R, TextureParameteriParam::CLAMP_TO_EDGE);
//
//  fbo.AttachTexture(FramebufferAttachment::DEPTH, texture.id, 0);
//  return fbo;
//}

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
    "GameEngine",                         // window title
    vec2i{ 16, 9 },                       // window aspect ratio
    false                                 // window v-sync
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

  // Initialize uniform block objects
  // --------------------------------
  // Init UBO CameraBlock
  {
    // Reserve memory for:
    // - 1 projection matrix
    // - 1 view matrix 
		constexpr u32 size = sizeof(mat4f) * 2;
		void* data = Array<mat4f, 2>{ mat4f(1.0f), mat4f(1.0f) }.data();
    _uboCameraBlock = Buffer(size,									    // Reserve memory for 2 mat4f
														 data,  							      // Init with identity matrices
                             BufferUsage::DYNAMIC_DRAW);// Data store content will be modified repeatedly and used many times.

    _uboCameraBlock.BindBase(BufferTarget::UNIFORM, 0); // "CameraBlock" to binding point 0
  }
  // Init UBO LightBlock
  {
    // Reserve memory for:
    // - 1 DirectionalLight object 
    // - 1 PointLight object 
    // - 1 SpotLight object
    constexpr u32 size = sizeof(DirectionalLight) + sizeof(PointLight) + sizeof(SpotLight);
    _uboLightBlock = Buffer(size,
                            nullptr,
                            BufferUsage::DYNAMIC_DRAW // Data store content will be modified repeatedly and used many times.
    );

		constexpr Array<u8, sizeof(DirectionalLight)> zeros{};
		_uboLightBlock.UpdateStorage(0, sizeof(DirectionalLight), zeros.data()); // Init with zeros
    _uboLightBlock.BindBase(BufferTarget::UNIFORM, 1); // "LightBlock" to binding point 1
  }
  // Init UBO BoneBlock
  {
		constexpr u32 size = SkeletalMesh::GetMaxNumBones() * sizeof(mat4f);
    _uboBoneBlock = Buffer(size,
                           nullptr,
                           BufferUsage::STREAM_DRAW // Data store content will be modified repeatedly and used many times.
    );
    constexpr Array<char, size> zeros{};
		_uboBoneBlock.UpdateStorage(0, size, zeros.data()); // Init buffer with zeros
    _uboBoneBlock.BindBase(BufferTarget::UNIFORM, 2);   // "BoneBlock" to binding point 2
  }

  // Set the initial OpenGL states
  // -----------------------------
  SetOpenGLStates();
}
void Engine::Run()
{
  // Create framebuffer
  CreateFramebuffer(4, WINDOW_WIDTH, WINDOW_HEIGHT);
  CreateScreenSquare();

	// Create grid plane mesh
	CreateGridPlane();

	// Create skybox mesh
  TextureCubemap skyboxTexture = CreateSkybox();

  // Create primary camera object
  Camera primaryCamera(vec3f(7.f, 4.f, 6), vec3f(-135.0f, -25.0f, 0.f));
  primaryCamera.frustum.zFar = 100.0f;

  Scene scene((Paths::GetRootPath() / "Scene.yaml"));

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  ImGuiLayer& gui = ImGuiLayer::Get();
  WindowManager& windowManager = WindowManager::Get();
  TexturesManager& texturesManager = TexturesManager::Get();
  ShadersManager& shadersManager = ShadersManager::Get();
  
  Program gridPlaneProgram = shadersManager.GetProgram("GridPlane");
  Program skyboxProgram = shadersManager.GetProgram("Skybox");
  skyboxProgram.SetUniform1i(Uniforms::skyboxTexture, 0);
  Program blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  blinnPhongProgram.SetUniform1i("u_material.diffuseTexture", 0);
  blinnPhongProgram.SetUniform1i("u_material.specularTexture", 1);
  blinnPhongProgram.SetUniform1i("u_material.normalTexture", 2);
  Program goochProgram = shadersManager.GetProgram("GoochShading");
  goochProgram.SetUniform1i("u_material.diffuseTexture", 0);
  goochProgram.SetUniform1i("u_material.specularTexture", 1);
  goochProgram.SetUniform1i("u_material.normalTexture", 2);

  constexpr bool wireframeMode = false;

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
      primaryCamera.ProcessKeyboard(delta, 5.0f);
      primaryCamera.ProcessMouse(delta, 15.0f);
    }

    // --------------------------------------------------------------------
    // -------------------------- Update section --------------------------
    // --------------------------------------------------------------------
    primaryCamera.UpdateOrientation();
    mat4f cameraView = primaryCamera.CalculateView(primaryCamera.position + primaryCamera.GetFrontVector());
    mat4f cameraProj = primaryCamera.CalculatePerspective(static_cast<f32>(_viewportSize.x) / static_cast<f32>(_viewportSize.y));
    
    // Update camera UBO
    {
      void* data = Array<mat4f, 2>{ cameraView, cameraProj }.data();
      _uboCameraBlock.UpdateStorage(0, sizeof(mat4f) * 2, data);
    }

    // Update light UBO
    {
      constexpr u32 size = sizeof(DirectionalLight) + sizeof(PointLight) + sizeof(SpotLight);
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

      e = scene.FindObjectWithComponent<SpotLight>();
      if (e.has_value())
      {
        GameObject obj = e.value();
        SpotLight* light = obj.GetComponent<SpotLight>();
        _uboLightBlock.UpdateStorage(sizeof(DirectionalLight) + sizeof(PointLight),
                                     sizeof(SpotLight),
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

      glPolygonMode(GL_FRONT, wireframeMode ? GL_LINE : GL_FILL);
      glPolygonMode(GL_BACK, wireframeMode ? GL_LINE : GL_FILL);

      /// Render scene here
    #if 1
      {
        goochProgram.Use();
        goochProgram.SetUniform3f(Uniforms::viewPos, primaryCamera.position);
        scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform)
        {
          transform.position.y = -2.0f;
          transform.UpdateTransformation();

          goochProgram.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
          staticMesh.Render(goochProgram, RenderMode::TRIANGLES);
        });

				blinnPhongProgram.Use();
        blinnPhongProgram.SetUniform3f(Uniforms::viewPos, primaryCamera.position);
        scene.Reg().view<StaticMesh, Transform>().each([&](auto& staticMesh, auto& transform) {
          transform.position.y = 2.0f;
          transform.UpdateTransformation();

          blinnPhongProgram.SetUniformMat4f(Uniforms::model, transform.GetTransformation());
          staticMesh.Render(blinnPhongProgram, RenderMode::TRIANGLES);
        });
      }
    #endif

      /// Render the infinite grid
    #if 1
      {
        gridPlaneProgram.Use();
        Renderer::DrawArrays(RenderMode::TRIANGLES, _gridPlane.vao, _gridPlane.numVertices);
      }
    #endif

      /// Draw skybox after the scene
    #if 0
      {
        skyboxProgram.Use();
        skyboxTexture.BindTextureUnit(0);
        DepthTest::SetDepthFun(CompareFunc::LEQUAL);
        Renderer::DrawArrays(RenderMode::TRIANGLES, _skybox.vao, _skybox.numVertices);
        DepthTest::SetDepthFun(CompareFunc::LESS);
      }
    #endif

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
    gui.RenderTimeInfo(delta, avgTime, frameRate);
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
  _skybox.Destroy();
  _gridPlane.Destroy();

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
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
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
void Engine::CreateGridPlane()
{
  constexpr f32 vertices[] = {
       1.f,  1.f, 0.f,
      -1.f, -1.f, 0.f,
      -1.f,  1.f, 0.f,
      -1.f, -1.f, 0.f,
       1.f,  1.f, 0.f,
       1.f, -1.f, 0.f
  };
  Buffer gridVbo(sizeof(vertices), vertices, BufferUsage::STATIC_DRAW);

	_gridPlane.Create();
  _gridPlane.numVertices = 6;
  _gridPlane.numIndices = 0;
  _gridPlane.vao.EnableAttribute(0);
  _gridPlane.vao.SetAttribBinding(0, 0);
  _gridPlane.vao.SetAttribFormatFLoat(0, 3, VertexAttribType::FLOAT, false, 0);
  _gridPlane.vao.AttachVertexBuffer(0, gridVbo, 0, 3 * sizeof(f32));
}
TextureCubemap Engine::CreateSkybox()
{
  constexpr f32 vertices[] = {
    // Position
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  Buffer vbo(sizeof(vertices), vertices, BufferUsage::STATIC_DRAW);

  _skybox.Create();
  _skybox.numVertices = 36;
  _skybox.numIndices = 0;

  _skybox.vao.EnableAttribute(0);
  _skybox.vao.SetAttribBinding(0, 0);
  _skybox.vao.SetAttribFormatFLoat(0, 3, VertexAttribType::FLOAT, false, 0);
  _skybox.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P));

  TexturesManager& texturesManager = TexturesManager::Get();
  Array<Texture2D, 6> images = {
    texturesManager.GetOrCreateTexture("skybox/right.jpg"),
    texturesManager.GetOrCreateTexture("skybox/left.jpg"),
    texturesManager.GetOrCreateTexture("skybox/top.jpg"),
    texturesManager.GetOrCreateTexture("skybox/bottom.jpg"),
    texturesManager.GetOrCreateTexture("skybox/front.jpg"),
    texturesManager.GetOrCreateTexture("skybox/back.jpg"),
  };
  Texture2DInternalFormat cubemapInternalFormat = images.at(0).GetInternalFormat();
  i32 width = images.at(0).GetWidth();
  i32 height = images.at(0).GetHeight();

	TextureCubemap skyboxTexture;
  skyboxTexture.Create();
  skyboxTexture.CreateStorage(cubemapInternalFormat, width, height);
  skyboxTexture.LoadImages(images);
  skyboxTexture.SetParameteri(TextureParameteriName::MAG_FILTER, TextureParameteriParam::LINEAR);
  skyboxTexture.SetParameteri(TextureParameteriName::MIN_FILTER, TextureParameteriParam::LINEAR);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_S, TextureParameteriParam::CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_T, TextureParameteriParam::CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(TextureParameteriName::WRAP_R, TextureParameteriParam::CLAMP_TO_EDGE);
  return skyboxTexture;
}
