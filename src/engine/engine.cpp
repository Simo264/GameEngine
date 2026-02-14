#include "engine.hpp"

#include "utils/logger.hpp"

#include "engine/globals.hpp"
#include "engine/paths.hpp"

#include "engine/scene_serializer.hpp"
#include "engine/graphics/renderbuffer.hpp"
#include "engine/graphics/render_api.hpp"
#include "engine/render_system.hpp"

#include "engine/managers/window_manager.hpp"
#include "engine/managers/shaders_manager.hpp"
#include "engine/managers/textures_manager.hpp"

#include "gui/imgui_layer.hpp"

#include <glad/gl.h>

using namespace Components;

constexpr auto INITIAL_WINDOW_W = 1600;
constexpr auto INITIAL_WINDOW_H = 900;
constexpr auto INITIAL_WINDOW_X = 50;
constexpr auto INITIAL_WINDOW_Y = 50;

static void GLAPIENTRY message_callback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       [[maybe_unused]] GLsizei length,
                                       const GLchar* message,
                                       [[maybe_unused]] const void* user_param)
{
  // Ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  const char* source_str;
  const char* type_str;
  const char* severity_str;
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    source_str = "API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    source_str = "Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    source_str = "Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    source_str = "Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    source_str = "Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    source_str = "Other";
    break;
  default:
    source_str = "Unknown";
    break;
  }

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    type_str = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    type_str = "Deprecated Behavior";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    type_str = "Undefined Behavior";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    type_str = "Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    type_str = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    type_str = "Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    type_str = "Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    type_str = "Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    type_str = "Other";
    break;
  default:
    type_str = "Unknown";
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

void Engine::Engine()
{
  // 1. Logger (non dipende da nulla)
  Logger::initialize();
  CONSOLE_INFO("Logger initialized");

  // 2. Window + OpenGL context
  auto props = WindowProps{};
  props.aspectRatio = vec2i{ 16, 9 };
  props.size = vec2i{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };
  props.position = vec2i{ INITIAL_WINDOW_X, INITIAL_WINDOW_Y };
  props.title = "GameEngine";
  props.vsync = false;
  props.contextVersionMajor = 4;
  props.contextVersionMinor = 6;
  props.samples = 4;
  WindowManager::GetInstance().Initialize(props);
  CONSOLE_INFO("Window manager initialized");

  // 3. OpenGL debug (subito dopo context creation)
  __SetupOpenGLDebug();
  __SetupOpenGLState();
  CONSOLE_INFO("OpenGL initialized");

  // 4. Scene (prima dei manager che potrebbero dipendere da essa)
  _scene.InitializeDefaultArchetypes();
  CONSOLE_INFO("Scene initialized");

  // 5. Resource managers
  ShadersManager::GetInstance().Initialize();
  CONSOLE_INFO("Shaders manager initialized");

  TexturesManager::GetInstance().Initialize();
  CONSOLE_INFO("Textures manager initialized");

  // 6. Rendering resources
  _MSAAFramebufferSamples = props.samples;  // Usa valore da props!
  __CreateMSAAFramebuffer(INITIAL_WINDOW_W, INITIAL_WINDOW_H, _MSAAFramebufferSamples);
  _screenQuad = __CreateMeshQuad();
  CONSOLE_INFO("Rendering resources initialized");

  // 7. Uniform buffers
  __CreateCameraUBO(0);  // "CameraBlock" -> binding 0
  __CreateLightUBO(1);   // "LightBlock" -> binding 1
  CONSOLE_INFO("Uniform buffers created");

  // 8. ImGui (dopo tutto il rendering setup)
  ImGuiLayer::GetInstance().InitializeImGui(_scene);
  CONSOLE_INFO("ImGui layer initialized");

  // 9. Time (ultimo, non dipende da nulla)
  __InitTime();
}

void Engine::run()
{
  auto viewportSize = Vec2I{ INITIAL_WINDOW_W, INITIAL_WINDOW_H };
  auto aspect = static_cast<f32>(viewportSize.x) / viewportSize.y;

  auto serializer = SceneSerializer{ _scene };
  serializer.Load(GetRootPath() / "Scene.yaml");

  auto e = _scene.FindEntityWithComponent<Camera>();
  auto& camera = _scene.GetEntityComponent<Camera>(e.value());
  e = _scene.FindEntityWithComponent<DirectionalLight>();
  auto& dirLight = _scene.GetEntityComponent<DirectionalLight>(e.value());
  e = _scene.FindEntityWithComponent<PointLight>();
  auto& pointLight = _scene.GetEntityComponent<PointLight>(e.value());
  e = _scene.FindEntityWithComponent<SpotLight>();
  auto& spotLight = _scene.GetEntityComponent<SpotLight>(e.value());

  // ----------------------------------------------------------------------
  // -------------------------- Pre-loop section --------------------------
  // ----------------------------------------------------------------------
  auto& guiLayer = ImGuiLayer::GetInstance();
  auto& windowManager = WindowManager::GetInstance();
  auto& shadersManager = ShadersManager::GetInstance();
  auto goochProgram = shadersManager.GetProgram("GoochShading");
  auto blinnPhongProgram = shadersManager.GetProgram("BlinnPhongShading");
  
  auto renderSystem = RenderSystem{};
  renderSystem.SetProgram(blinnPhongProgram);

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
    if (guiLayer.viewport.isFocused)
    {
      camera.ProcessKeyboard(static_cast<f32>(_delta), 10.0f);
      camera.ProcessMouseMovement(0.05f);
    }

    // --------------------------------------------------------------------
    // -------------------------- Update section --------------------------
    // --------------------------------------------------------------------
    auto cameraView = camera.CalculateViewMatrix();
    auto cameraProj = camera.CalculatePerspectiveMatrix(aspect);

    // Update camera UBO
    {
      auto matrices = Array<Mat4F, 2>{ cameraView, cameraProj };
      _uboCameraBlock.UpdateStorage(0, sizeof(matrices), reinterpret_cast<byte*>(matrices.data()));
      _uboCameraBlock.UpdateStorage(sizeof(matrices), sizeof(Vec3F), reinterpret_cast<byte*>(&camera.position));
    }

    // Update light UBO
    {
      auto offset = 0;
      _uboLightBlock.UpdateStorage(offset, sizeof(DirectionalLight), reinterpret_cast<byte*>(&dirLight));
      offset += sizeof(DirectionalLight);
      _uboLightBlock.UpdateStorage(offset, sizeof(PointLight), reinterpret_cast<byte*>(&pointLight));
      offset += sizeof(PointLight);
      _uboLightBlock.UpdateStorage(offset, sizeof(SpotLight), reinterpret_cast<byte*>(&spotLight));
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
      
      renderSystem.Render(_scene);

      glDisable(GL_MULTISAMPLE);
    }
    _MSAAFramebuffer.Unbind(FramebufferTarget::DRAW);

    // 2. Resolve framebuffer
    __ResolveMSAAFramebuffer(viewportSize.x, viewportSize.y);

    // 3. Get the final texture color image 
    auto viewportImage = _MSAAFramebufferResolver.GetTextureAttachment(0); // returns the texture color 

    guiLayer.menubar.Render();
    guiLayer.ImguiDemo();
    guiLayer.hierarchy.Render();
    auto entitySelected = guiLayer.hierarchy.target;
    guiLayer.viewport.Render("Viewport", viewportImage, entitySelected, cameraView, cameraProj);
    guiLayer.inspector.Render("Inspector", entitySelected);
    guiLayer.toolbar.Render("Toolbar", guiLayer.viewport.position, guiLayer.viewport.size, guiLayer.viewport.gizmoOp);

    guiLayer.DebugInfo(_delta, _avgTime, _frameRate);
    guiLayer.GraphicsInfo();
    guiLayer.CompleteFrameRender();

    // Checking viewport size
    if (viewportSize != guiLayer.viewport.size)
    {
      viewportSize = guiLayer.viewport.size;
      aspect = static_cast<f32>(viewportSize.x) / viewportSize.y;
      
      _MSAAFramebufferResolver.Release();
      _MSAAFramebuffer.Release();
      __CreateMSAAFramebuffer(viewportSize.x, viewportSize.y, _MSAAFramebufferSamples);
    }

    // ------------------------------------------------------------------
    // -------------------------- Swap buffers --------------------------
    // ------------------------------------------------------------------
    windowManager.SwapWindowBuffers();
  }
}

Engine::~Engine()
{
  scene_.clear();

  screen_quad_.release();
  ubo_camera_block_.release();
  ubo_light_block_.release();
  ubo_material_block_.release();
  msaa_framebuffer_.release();
  msaa_framebuffer_resolver_.release();

  ImGuiLayer::instance().cleanup_imgui();
  ShadersManager::instance().cleanup();
  TexturesManager::instance().cleanup();
  WindowManager::instance().cleanUp();
}


// -----------------------------------------------------
//            PRIVATE METHODS
// -----------------------------------------------------

void Engine::__SetupOpenGLDebug() const
{
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
void Engine::__SetupOpenGLState() const
{
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // specify clear values for the color buffers
  glClearDepth(1.0f);                       // specify clear values for the depth buffer    
  glClearStencil(0);                        // specify clear values for the stencil buffer 

  // Depth testing ON
  // ----------------
  RenderAPI::EnableDepthTest();
  RenderAPI::EnableWritingDepthBuffer();
  RenderAPI::SetDepthFun(CompareFunc::LESS);

  // Stencil testing OFF
  // -------------------
  RenderAPI::DisableStencilTest();

  // Culling OFF
  // -----------
  RenderAPI::DisableFaceCulling();

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

StaticMesh Engine::__CreateMeshQuad()
{
  using Vertex = VertexLayout<Position, TextureCoord>;
  constexpr auto vertex1 = Vertex{ Vec3F(-1.0f,  1.0f, 0.0f), Vec2F(0.0f, 1.0f) };
  constexpr auto vertex2 = Vertex{ Vec3F(-1.0f, -1.0f, 0.0f), Vec2F(0.0f, 0.0f) };
  constexpr auto vertex3 = Vertex{ Vec3F(1.0f,  1.0f, 0.0f),  Vec2F(1.0f, 1.0f) };
  constexpr auto vertex4 = Vertex{ Vec3F(1.0f, -1.0f, 0.0f),  Vec2F(1.0f, 0.0f) };
  constexpr auto vertices = Array<Vertex, 4>{ vertex1,vertex2,vertex3,vertex4, };

  constexpr auto stride = sizeof(Vertex);
  constexpr auto offsetPosition = 0;
  constexpr auto offsetTC = offsetPosition + sizeof(Position);
  constexpr auto vertexFormat0 = VertexFormat{ 3, VertexAttribType::FLOAT, false, static_cast<i32>(offsetPosition) };
  constexpr auto vertexFormat1 = VertexFormat{ 2, VertexAttribType::FLOAT, false, static_cast<i32>(offsetTC) };

  auto quad = StaticMesh{};
  quad.Create();
  quad.vertexArray.SetupVertexAttribute(0, 0, vertexFormat0);
  quad.vertexArray.SetupVertexAttribute(1, 0, vertexFormat1);
  auto& vBuffer = quad.vertexBuffer;
  vBuffer.CreateImmutableStorage(sizeof(vertices), vertices.data(), BufferStorageFlags::NONE);
  quad.vertexArray.AttachVertexBuffer(0, vBuffer.id, 0, stride);
  return quad;
}
void Engine::__CreateMSAAFramebuffer(i32 w, i32 h, i32 samples)
{
  if (_MSAAFramebuffer.Valid())
    CONSOLE_WARN("Warning in __CreateMSAAFramebuffer: _MSAAFramebuffer.IsValid()");
  if (_MSAAFramebufferResolver.Valid())
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
