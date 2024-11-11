#include "Engine.hpp"

#include "Core/GL.hpp"
#include "Core/Math/Ext.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"

#include "Engine/ECS/ECS.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/DepthTest.hpp"
#include "Engine/Graphics/StencilTest.hpp"
#include "Engine/Graphics/FaceCulling.hpp"
#include "Engine/Graphics/Objects/RenderBuffer.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Graphics/Objects/TextureCubemap.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"
#include "Engine/Subsystems/FontManager.hpp"

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static chrono::steady_clock::time_point now{};
static chrono::steady_clock::time_point lastFrameTime{};
static f64 delta = 0.0f; /* Time elapsed between two frames */

static chrono::steady_clock::time_point timerT0 = chrono::steady_clock::now();
static chrono::steady_clock::time_point timerT1{};
static i32 frames = 0;
static i32 frameRate = 0; /* How many frames generated per seconds */
static f64 totalDeltasPerSecond = 0.0f;
static f64 avgTime = 0.0f; /* The average rendering time per seconds */

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  /* Ignore non-significant error/warning codes */ 
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
    return;

  CONSOLE_ERROR("GL callback: {} ", message);
}
static void SetOpenGLStates()
{
  /* Enable debug output */ 
  /* ------------------- */
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

  /* Depth testing ON */
  /* ---------------- */
  DepthTest::EnableTest();
  DepthTest::EnableWritingBuffer();
  DepthTest::SetDepthFun(DepthFun::LESS);

  /* Stencil testing OFF */
  /* ------------------- */
  StencilTest::DisableTest();
  StencilTest::SetStencilFun(StencilFun::ALWAYS, 0, 0xFF);
  StencilTest::SetStencilOp(StencilOp::KEEP, StencilOp::KEEP, StencilOp::KEEP);

  /* Culling OFF */
  /* ----------- */
  FaceCulling::DisableFaceCulling();
  FaceCulling::SetCullFace(CullFace::BACK);
  FaceCulling::SetFrontFacing(FrontFace::COUNTER_CLOCKWISE);

  /* Blending OFF */
  /* ------------ */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Gamma correction OFF */
  /* -------------------- */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  /* -------------- */
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

static void RenderDirectionalLight(Program& program, const DirectionalLight& light)
{
  program.SetUniform3f("u_directionalLight.color", light.color);
  program.SetUniform1f("u_directionalLight.intensity", light.intensity);
  program.SetUniform3f("u_directionalLight.direction", light.direction);
}
static void RenderPointLight(Program& program, const PointLight& light)
{
  program.SetUniform3f("u_pointLight.color", light.color);
  program.SetUniform1f("u_pointLight.intensity", light.intensity);
  program.SetUniform3f("u_pointLight.position", light.position);
  program.SetUniform1f("u_pointLight.attenuation.kl", light.attenuation.kl);
  program.SetUniform1f("u_pointLight.attenuation.kq", light.attenuation.kq);

#if 0
  program.SetUniform3f(std::format("u_pointLight[{}].color", i).c_str(), light.color);
  program.SetUniform1f(std::format("u_pointLight[{}].intensity", i).c_str(), light.intensity);
  program.SetUniform3f(std::format("u_pointLight[{}].position", i).c_str(), light.position);
  program.SetUniform1f(std::format("u_pointLight[{}].attenuation.kl", i).c_str(), light.attenuation.kl);
  program.SetUniform1f(std::format("u_pointLight[{}].attenuation.kq", i).c_str(), light.attenuation.kq);
#endif
}
static void RenderSpotLight(Program& program, const SpotLight& light)
{
  program.SetUniform3f("u_spotLight.color", light.color);
  program.SetUniform1f("u_spotLight.intensity", light.intensity);
  program.SetUniform3f("u_spotLight.direction", light.direction);
  program.SetUniform3f("u_spotLight.position", light.position);
  program.SetUniform1f("u_spotLight.attenuation.kl", light.attenuation.kl);
  program.SetUniform1f("u_spotLight.attenuation.kq", light.attenuation.kq);
  program.SetUniform1f("u_spotLight.cutOff", light.cutOff);
  program.SetUniform1f("u_spotLight.outerCutOff", light.outerCutOff);
}
static void RenderScene(Scene& scene, Program& program)
{
  scene.Reg().view<DirectionalLight>().each([&](auto& light) {
    RenderDirectionalLight(program, light);
  });

  //i32 i = 0;
  scene.Reg().view<PointLight>().each([&](auto& light) {
    RenderPointLight(program, light);
    //i++;
  });

  scene.Reg().view<SpotLight>().each([&](auto& light) {
    RenderSpotLight(program, light);
  });

  scene.Reg().view<Model, Transform>().each([&](auto& model, auto& transform) {
    program.SetUniformMat4f("u_model", transform.GetTransformation());
    model.DrawModel(GL_TRIANGLES);
  });
}

static Mesh CreateSkybox(TextureCubemap& skyboxTexture)
{
  constexpr f32 vertices[] = {
    /* Position */
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
  Buffer vbo(sizeof(vertices), vertices, GL_STATIC_DRAW);

  Mesh skybox;
  skybox.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, 0));
  skybox.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P));
  skybox.vao.numVertices = 36;

  TextureManager& textureManager = TextureManager::Get();
  Array<Texture2D*, 6> images = {
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/right.jpg"),
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/left.jpg"),
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/top.jpg"),
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/bottom.jpg"),
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/front.jpg"),
    &textureManager.GetTextureByPath(GetTexturesPath() / "skybox/back.jpg"),
  };
  i32 cubemapInternalFormat = images.at(0)->internalFormat;
  i32 width = images.at(0)->width;
  i32 height = images.at(0)->height;

  skyboxTexture.Create();
  skyboxTexture.CreateStorage(cubemapInternalFormat, width, height);
  skyboxTexture.LoadImages(images);
  skyboxTexture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return skybox;
}
static FrameBuffer CreateDepthMapFbo(i32 width, i32 height)
{
  /* Create a 2D texture that we'll use as the framebuffer's depth buffer */
  Texture2D depthMap;
  depthMap.Create(GL_TEXTURE_2D);
  depthMap.CreateStorage(GL_DEPTH_COMPONENT24, width, height);
  depthMap.SetParameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  depthMap.SetParameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  depthMap.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  depthMap.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* Resolve the problem of over sampling */
  depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  depthMap.SetParameterfv(GL_TEXTURE_BORDER_COLOR, Array<f32, 4>{ 1.0, 1.0, 1.0, 1.0 }.data());

  /* With the generated depth texture we can attach it as the framebuffer's depth buffer */
  FrameBuffer fbo;
  fbo.Create();
  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, depthMap.id, 0);
  return fbo;
}
static FrameBuffer CreateDepthCubeMapFbo(i32 width, i32 height)
{
  FrameBuffer fbo;
  fbo.Create();

  TextureCubemap texture;
  texture.Create();
  texture.CreateStorage(GL_DEPTH_COMPONENT24, width, height);
  for (i32 i = 0; i < 6; i++)
    texture.SubImage3D(0, 0, 0, i, width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, texture.id, 0);
  return fbo;
}
static Mesh CreateTerrain(i32 rez)
{
  auto& texManager = TextureManager::Get();
  Texture2D& heightMap = texManager.GetTextureByPath(GetTexturesPath() / "iceland_heightmap.png");
  i32 width = heightMap.width;
  i32 height = heightMap.height;

  // vertex generation
  Vector<f32> vertices;
  for (i32 i = 0; i <= rez - 1; i++)
  {
    for (i32 j = 0; j <= rez - 1; j++)
    {
      vertices.push_back(-width / 2.0f + width * i / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * j / (f32)rez); // v.z
      vertices.push_back(i / (f32)rez); // u
      vertices.push_back(j / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * j / (f32)rez); // v.z
      vertices.push_back((i + 1) / (f32)rez); // u
      vertices.push_back(j / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * i / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / (f32)rez); // v.z
      vertices.push_back(i / (f32)rez); // u
      vertices.push_back((j + 1) / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / (f32)rez); // v.z
      vertices.push_back((i + 1) / (f32)rez); // u
      vertices.push_back((j + 1) / (f32)rez); // v
    }
  }
  Buffer vbo(vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);
  
  Mesh terrain;
  terrain.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, offsetof(Vertex_P_UV, position)));
  terrain.SetupAttributeFloat(1, 0, VertexFormat(2, GL_FLOAT, false, offsetof(Vertex_P_UV, uv)));
  terrain.vao.AttachVertexBuffer(0, vbo, 0, sizeof(Vertex_P_UV));
  terrain.vao.numVertices = 4 * pow(rez, 2);
  return terrain;
}
static Mesh CreateGridPlane()
{
  constexpr f32 vertices[] = {
       1.f,  1.f, 0.f,
      -1.f, -1.f, 0.f,
      -1.f,  1.f, 0.f,
      -1.f, -1.f, 0.f,
       1.f,  1.f, 0.f,
       1.f, -1.f, 0.f
  };
  Buffer gridVbo(sizeof(vertices), vertices, GL_STATIC_DRAW);

  Mesh grid;
  grid.SetupAttributeFloat(0, 0, VertexFormat(3, GL_FLOAT, false, 0));
  grid.vao.AttachVertexBuffer(0, gridVbo, 0, 3 * sizeof(f32));
  grid.vao.numVertices = 6;

  return grid;
}

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  /* Initialize logger */
  /* ----------------- */
  Logger::Initialize();

  /* Initialize window manager */
  /* ------------------------- */
  WindowManager::Get().Initialize(WindowProps(
    vec2i{ WINDOW_WIDTH, WINDOW_HEIGHT },
    vec2i{ 50, 50 }, 
    "GameEngine",
    vec2i{ 16, 9 },
    false
  ));

  /* Initialize font manager */
  /* -------------------------- */
  FontManager::Get().Initialize();
  
  /* Initialize shader manager */
  /* ------------------------- */
  ShaderManager::Get().Initialize();

  /* Initialize texture manager */
  /* -------------------------- */
  TextureManager::Get().Initialize();

  /* Setup ImGui context */
  /* ------------------- */
  ImGuiLayer::Get().Initialize();
  CONSOLE_INFO("ImGui layer initialized");

  /* Create Framebuffer object */
  /* ------------------------- */
  _viewportSize = { WINDOW_WIDTH, WINDOW_HEIGHT };
  CreateFramebuffer(4, _viewportSize.x, _viewportSize.y);
  CreateScreenSquare();

  /* Initialize uniform block objects */
  /* -------------------------------- */
  /* Reserve memory for both projection,view matrices */
  _uboCamera = Buffer(2 * sizeof(mat4f), nullptr, GL_STATIC_DRAW);
  _uboCamera.BindBase(GL_UNIFORM_BUFFER, 0); /* cameraBlock to bindingpoint 0 */

  /* Set the initial OpenGL states */
  /* ----------------------------- */
  SetOpenGLStates();
}
void Engine::Run()
{
  /* Create scene object */
  Scene scene((GetRootPath() / "Scene.ini"));

  DirectionalLight* directionalLight = nullptr;
  scene.Reg().view<DirectionalLight>().each([&](auto& light) { directionalLight = &light; });

  /* Create primary camera object */
  Camera primaryCamera(vec3f(7.f, 4.f, 6), vec3f(-135.0f, -25.0f, 0.f));
  primaryCamera.frustum.zFar = 100.0f;

  Skeleton skeleton(GetSkeletalsPath() / "sk_walk_anim.fbx");
  Animation animation(GetSkeletalsPath() / "sk_walk_anim.fbx", skeleton);
  Animator animator(&animation);

  /* ---------------------------------------------------------------------- */
  /* -------------------------- Pre-loop section -------------------------- */
  /* ---------------------------------------------------------------------- */
  WindowManager& windowManager = WindowManager::Get();
  ImGuiLayer& gui = ImGuiLayer::Get();
  TextureManager& textureManager = TextureManager::Get();
  ShaderManager& shaderManager = ShaderManager::Get();
  Program& skyboxProgram = shaderManager.GetProgramByName("Skybox");
  Program& gridPlaneProgram = shaderManager.GetProgramByName("GridPlane");
  Program& skeletalAnimProgram = shaderManager.GetProgramByName("SkeletalAnim");
  
  /* ------------------------------------------------------------------ */
  /* -------------------------- loop section -------------------------- */
  /* ------------------------------------------------------------------ */
  while (windowManager.IsOpen())
  {
    /* Set new font before BeginFrame */
    if (gui.changeFontFamily)
    {
      gui.SetFont(*gui.selectedFont.second);
      gui.changeFontFamily = false;
    }
    gui.BeginFrame();

    /* ---------------------------------------------------------------------------------- */
    /* -------------------------- Per-frame time logic section -------------------------- */
    /* ---------------------------------------------------------------------------------- */
    CalculatePerFrameTime();
    g_drawCalls = 0;

    /* ------------------------------------------------------------------- */
    /* -------------------------- Input section -------------------------- */
    /* ------------------------------------------------------------------- */
    windowManager.PoolEvents();
    if (gui.viewportFocused)
    {
      primaryCamera.ProcessKeyboard(delta, 5.0f);
      primaryCamera.ProcessMouse(delta, 15.0f);
    }

    /* -------------------------------------------------------------------- */
    /* -------------------------- Update section -------------------------- */
    /* -------------------------------------------------------------------- */
    primaryCamera.UpdateOrientation();
    mat4f cameraView = primaryCamera.CalculateView(primaryCamera.position + primaryCamera.GetFrontVector());
    mat4f cameraProj = primaryCamera.CalculatePerspective(static_cast<f32>(_viewportSize.x) / static_cast<f32>(_viewportSize.y));
    _uboCamera.UpdateStorage(0, sizeof(cameraView), &cameraView[0]);
    _uboCamera.UpdateStorage(sizeof(cameraView), sizeof(cameraProj), &cameraProj[0]);
    
    //animator.UpdateAnimation(delta);

    /* ----------------------------------------------------------------------- */
    /* -------------------------- Rendering section -------------------------- */
    /* ----------------------------------------------------------------------- */

    /* Fill the framebuffer color texture */
    _fboMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, _viewportSize.x, _viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      skeletalAnimProgram.Use();
      skeletalAnimProgram.SetUniform3f("u_viewPos", primaryCamera.position);
      auto& transforms = animator.GetFinalBoneMatrices();
      for (u64 i = 0; i < transforms.size(); i++)
      {
        char uniform[64]{};
        std::format_to_n(uniform, sizeof(uniform), "u_finalBonesMatrices[{}]", i);
        skeletalAnimProgram.SetUniformMat4f(uniform, transforms[i]);
      }
      scene.Reg().view<DirectionalLight>().each([&](auto& light)  { RenderDirectionalLight(skeletalAnimProgram, light); });
        
      mat4f model = mat4f(1.f);
      model = glm::scale(model, vec3f(1.f/64.f));
      skeletalAnimProgram.SetUniformMat4f("u_model", model);
      skeleton.DrawSkeleton(GL_TRIANGLES);

      
      /* Blit multisampled buffer to normal color buffer of intermediate FBO */
      _fboMultisampled.Blit(_fboIntermediate,
        0, 0, _viewportSize.x, _viewportSize.y,
        0, 0, _viewportSize.x, _viewportSize.y,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
    }
    _fboMultisampled.Unbind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gui.MenuBar(scene);
    GameObject& objSelected = gui.Hierarchy(scene);
    gui.Inspector(objSelected);
    u32 fboTexture = _fboIntermediate.textAttachments.at(0);
    gui.Viewport(fboTexture, objSelected, cameraView, cameraProj);
    gui.ContentBrowser();
    gui.GizmoToolBar(objSelected);
    gui.TimeInfo(delta, avgTime, frameRate);
    gui.GraphicsInfo();
    gui.Demo();
    gui.EndFrame();

    /* Checking viewport size */
    if (_viewportSize != gui.viewportSize)
    {
      _viewportSize = gui.viewportSize;
      _fboMultisampled.Delete();
      _fboIntermediate.Delete();
      CreateFramebuffer(4, _viewportSize.x, _viewportSize.y);
    }

    /* ------------------------------------------------------------------ */
    /* -------------------------- Swap buffers -------------------------- */
    /* ------------------------------------------------------------------ */
    windowManager.SwapWindowBuffers();

    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
 
  scene.ClearScene();
}
void Engine::CleanUp()
{
  _fboIntermediate.Delete();
  _fboMultisampled.Delete();
  _screenSquare.Delete();
  _uboCamera.Delete();

  ImGuiLayer::Get().CleanUp();
  ShaderManager::Get().CleanUp();
  TextureManager::Get().CleanUp();
  WindowManager::Get().CleanUp(); /* !!Raise exception here */
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Engine::CreateFramebuffer(i32 samples, i32 width, i32 height)
{
  _fboMultisampled.Create();

  /* Create a multisampled color attachment texture */
  Texture2D textColMultAtt;
  textColMultAtt.Create(GL_TEXTURE_2D_MULTISAMPLE);
  textColMultAtt.CreateStorageMultisampled(GL_RGB8, samples, width, height);
  /* Create a multisampled renderbuffer object for depth and stencil attachments */
  RenderBuffer depthStencMultAtt;
  depthStencMultAtt.Create();
  depthStencMultAtt.CreateStorageMulstisampled(GL_DEPTH24_STENCIL8, samples, width, height);

  _fboMultisampled.AttachTexture(GL_COLOR_ATTACHMENT0, textColMultAtt.id, 0);
  _fboMultisampled.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthStencMultAtt.id);

  if (_fboMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  /* Configure second post - processing framebuffer */
  _fboIntermediate.Create();

  /* Create normal color attachment texture */
  Texture2D textColAtt;
  textColAtt.Create(GL_TEXTURE_2D);
  textColAtt.CreateStorage(GL_RGB8, width, height);
  textColAtt.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  textColAtt.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  _fboIntermediate.AttachTexture(GL_COLOR_ATTACHMENT0, textColAtt.id, 0);

  if (_fboIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
}
void Engine::CreateScreenSquare()
{
  _screenSquare.Create();

  constexpr f32 vertices[] = {
    /* position    uv */
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  Buffer vbo(sizeof(vertices), vertices, GL_STATIC_DRAW);
  _screenSquare.AttachVertexBuffer(0, vbo, 0, 4 * sizeof(f32));

  _screenSquare.SetAttribFormatFLoat(0, 2, GL_FLOAT, true, 0);
  _screenSquare.SetAttribBinding(0, 0);
  _screenSquare.EnableAttribute(0);

  _screenSquare.SetAttribFormatFLoat(1, 2, GL_FLOAT, true, 2 * sizeof(f32));
  _screenSquare.SetAttribBinding(1, 0);
  _screenSquare.EnableAttribute(1);
  
  _screenSquare.numVertices = 6;
  _screenSquare.numIndices = 0;
}
