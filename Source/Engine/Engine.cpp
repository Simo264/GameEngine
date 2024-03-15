#include "Engine.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/ECS/GameObject.hpp"
#include "Engine/ECS/Components.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  Logger::Initialize();

  WindowManager::Instance().Initialize();
  LoadConfig();

  ShaderManager::Instance().Initialize();
  LoadShaders();
  
  TextureManager::Instance().Initialize();
  LoadTextures();
  LoadIcons();

  editor.Initialize();
}

void Engine::Run()
{
  auto& window                = WindowManager::Instance();
  auto& instanceSM            = ShaderManager::Instance();
  auto& testingShader         = instanceSM.GetShader("TestingShader");
  auto& instancingShader      = instanceSM.GetShader("InstancingShader");
  auto& framebufferShader     = instanceSM.GetShader("FramebufferShader");
  auto& sceneShader           = instanceSM.GetShader("SceneShader");
  auto& shadowMapDepthShader  = instanceSM.GetShader("ShadowMapDepthShader");
  auto& shadowMapShader       = instanceSM.GetShader("ShadowMapShader");

  /* Create framebuffer */
  Vec2i framebufferSize = window.GetFramebufferSize();
  FrameBuffer framebuffer(framebufferSize);
  framebuffer.AttachMultisampledColorBuffers();
  framebuffer.AttachMultisampledDepthStencilBuffer();
  assert(framebuffer.CheckStatus() && "Framebuffer is not complete!");
  glViewport(0, 0, framebufferSize.x, framebufferSize.y);

  /* Create camera */
  Camera camera({ 0.0f, 0.0f, 5.0f }, 45.0f, framebuffer.GetAspect());

  /* Create scene */
  Scene scene;
  
 
  /* Pre-loop */
  TimePoint lastUpdateTime = SystemClock::now();
  const Vec2f perspectiveDistance(0.1f, 100.0f); 

  /* Loop  */
  while (window.IsOpen())
  {
    editor.Begin();

    /* Per-frame time logic */
    framebufferSize = framebuffer.GetSize();
    const auto now = SystemClock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    Renderer::drawCalls = 0;


    /* Input */
    window.PoolEvents();
    if (editor.viewportPanel->isFocused)
      camera.ProcessInput(window, delta);
      
    const auto& cameraViewMatrix        = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix  = camera.cameraComponent->GetProjection();

    /* Render to frame buffer */
    framebuffer.Bind();
    glClearColor(pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    sceneShader.Use();
    sceneShader.SetMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
    sceneShader.SetMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
    scene.DrawScene(sceneShader);

    framebuffer.Blit();

    editor.Render(scene, camera, framebuffer);

    framebuffer.Unbind();
    glClearColor(pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    editor.End();
    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
}

void Engine::CleanUp()
{
  ShaderManager::Instance().CleanUp();
  
  TextureManager::Instance().CleanUp();

  editor.CleanUp();

  WindowManager::Instance().CleanUp();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Engine::LoadConfig()
{
  INIFileParser conf(ROOT_PATH / CONFIG_FILENAME);
  conf.ReadData();

  String title = conf.GetValue("window", "title");

  String tmp;
  tmp.reserve(32);

  tmp = conf.GetValue("window", "resolution");
  Vec2i resolution = INIFileParser::StringToVec2i(tmp, "x");
  tmp = conf.GetValue("window", "aspectratio");
  Vec2i aspectratio = INIFileParser::StringToVec2i(tmp, ":");
  tmp = conf.GetValue("window", "position");
  Vec2i position = INIFileParser::StringToVec2i(tmp, ",");
  tmp = conf.GetValue("window", "vsync");
  bool vsync = INIFileParser::StringToBool(tmp);

  auto& window = WindowManager::Instance();
  window.SetWindowTitle(title.c_str());
  window.SetWindowSize(resolution.x, resolution.y);
  window.SetWindowPosition(position.x, position.y);
  window.SetWindowAspectRatio(aspectratio.x, aspectratio.y);
  window.SetWindowVsync(vsync);
}

void Engine::LoadShaders()
{
  auto& instanceSM = ShaderManager::Instance();
  auto& testingShader = instanceSM.LoadShader(
    "TestingShader",
    SHADERS_PATH / "Testing.vert",
    SHADERS_PATH / "Testing.frag");
  auto& instancingShader = instanceSM.LoadShader(
    "InstancingShader",
    SHADERS_PATH / "Instancing.vert",
    SHADERS_PATH / "Scene.frag");
  auto& sceneShader = instanceSM.LoadShader(
    "SceneShader",
    SHADERS_PATH / "Scene.vert",
    SHADERS_PATH / "Scene.frag");
  auto& framebufferShader = instanceSM.LoadShader(
    "FramebufferShader",
    SHADERS_PATH / "Framebuffer.vert",
    SHADERS_PATH / "Framebuffer.frag");
  auto& shadowMapDepthShader = instanceSM.LoadShader(
    "ShadowMapDepthShader",
    SHADERS_PATH / "ShadowMapDepth.vert",
    SHADERS_PATH / "ShadowMapDepth.frag");
  auto& shadowMapShader = instanceSM.LoadShader(
    "ShadowMapShader",
    SHADERS_PATH / "ShadowMap.vert",
    SHADERS_PATH / "ShadowMap.frag");

  framebufferShader.Use();
  framebufferShader.SetInt("UScreenTexture", 0);
  framebufferShader.SetInt("UPostProcessingType", 0);

  instancingShader.Use();
  instancingShader.SetInt("UMaterial.diffuse", 0);
  instancingShader.SetInt("UMaterial.specular", 1);
  instancingShader.SetFloat("UMaterial.shininess", 32.0f);
  instancingShader.SetFloat("UGamma", 2.2f);

  sceneShader.Use();
  sceneShader.SetInt("UMaterial.diffuse", 0);
  sceneShader.SetInt("UMaterial.specular", 1);
  sceneShader.SetFloat("UMaterial.shininess", 32.0f);
  sceneShader.SetFloat("UGamma", 2.2f);
  sceneShader.SetFloat("UDirLightVisible", true);
  sceneShader.SetIntArray("UPointLightVisible", 5, Array<int, 5>{1,1,1,1,1}.data());
}

void Engine::LoadTextures()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(TEXTURES_PATH))
    if (!std::filesystem::is_directory(entry))
      TextureManager::Instance().LoadTexture(entry);
}

void Engine::LoadIcons()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(ICONS_PATH))
    if (!std::filesystem::is_directory(entry))
      TextureManager::Instance().LoadTexture(entry);
}
