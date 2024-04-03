#include "Engine.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"
#include "Engine/ShadowMap.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/ShaderUniforms.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <GLFW/glfw3.h>

Vec3f lightPosition{ -2.0f, 4.0f, -1.0f };

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  Logger::Initialize();

  WindowManager::Instance().Initialize();
  LoadConfig();

  SetGLStates();

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
  auto& instanceTM            = TextureManager::Instance();
  auto& testingShader         = instanceSM.GetShader("TestingShader");
  auto& instancingShader      = instanceSM.GetShader("InstancingShader");
  auto& framebufferShader     = instanceSM.GetShader("FramebufferShader");
  auto& sceneShader           = instanceSM.GetShader("SceneShader");
  auto& shadowMapDepthShader  = instanceSM.GetShader("ShadowMapDepthShader");
  auto& shadowMapShader       = instanceSM.GetShader("ShadowMapShader");

#if 0
  auto& diffuseMap  = instanceTM.GetTextureByPath(TEXTURES_PATH / "container_diffuse.png");
  auto& specularMap = instanceTM.GetTextureByPath(TEXTURES_PATH / "container_specular.png");

  Camera camera({ 0.0f, 0.0f, 5.0f }, 45.0f, (16.0f/9.0f));
  
  float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  };

  Scene scene;
  GameObject cube = scene.CreateObject("Cube", 0);
  cube.AddComponent<TransformComponent>();
  auto& sm = cube.AddComponent<StaticMeshComponent>(sizeof(vertices), vertices, (uint32_t)0, nullptr);
  sm.material.diffuse = &diffuseMap;
  sm.material.specular = &specularMap;

  TimePoint lastUpdateTime = SystemClock::now();
  while (window.IsOpen())
  {
    const auto now = SystemClock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();

    window.PoolEvents();
    camera.ProcessInput(window, delta);
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    testingShader.Use();
    testingShader.SetInt("UMaterial.diffuse", 0);
    testingShader.SetInt("UMaterial.specular", 1);
    testingShader.SetVec3f("UViewPos", camera.cameraComponent->position);
    testingShader.SetVec3f("ULight.position", { 1.2f, 1.0f, 2.0f });
    testingShader.SetVec3f("ULight.ambient", { 0.2f, 0.2f, 0.2f });
    testingShader.SetVec3f("ULight.diffuse", { 0.5f, 0.5f, 0.5f });
    testingShader.SetVec3f("ULight.specular", { 1.0f, 1.0f, 1.0f });
    testingShader.SetFloat("UMaterial.shininess", 64.0f);
    testingShader.SetMat4f("UProjection", cameraProjectionMatrix);
    testingShader.SetMat4f("UView", cameraViewMatrix);
    testingShader.SetMat4f("UModel", Mat4f(1.0f));

    cube.GetComponent<StaticMeshComponent>()->DrawMesh();



    glDisable(GL_DEPTH_TEST);

    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
#endif


#if 1
  /* -------------------------- Framebuffer -------------------------- */
  Vec2i framebufferSize = window.GetFramebufferSize();
  FrameBuffer framebuffer(framebufferSize);
  const uint32_t& framebufferImage = framebuffer.GetImage();


  /* -------------------------- Camera -------------------------- */
  Camera camera({ 0.0f, 0.0f, 5.0f }, 45.0f, framebuffer.GetAspect());


  /* -------------------------- Scene -------------------------- */
  Scene scene;
  scene.LoadScene(ROOT_PATH / "Scene.ini");
 

  /* -------------------------- Shadow mapping -------------------------- */
  constexpr int shadowWidth = 1024, shadowHeight = 1024;
  ShadowMap shadowMapping{ shadowWidth,shadowHeight };
  const uint32_t depthMapTexture = shadowMapping.GetDepthTexture();


  /* -------------------------- Pre-loop -------------------------- */
  TimePoint lastUpdateTime = SystemClock::now();
  const double colorValue = Math::Pow(static_cast<double>(0.3), static_cast<double>(GAMMA_CORRECTION));

  Mat4f lightSpaceMatrix{};

  /* -------------------------- Loop -------------------------- */
  while (window.IsOpen())
  {
    editor.Begin();

    /* -------------------------- Per-frame time logic -------------------------- */
    framebufferSize = framebuffer.GetSize();
    const auto now = SystemClock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    Renderer::drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    window.PoolEvents();
    if (editor.viewportPanel->isFocused)
      camera.ProcessInput(window, delta);
    const auto& cameraViewMatrix        = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix  = camera.cameraComponent->GetProjection();
    
    /* -------------------------- Rendering -------------------------- */
    
    /* Fill depth buffer into the ShadowMap texture */
    glViewport(0, 0, shadowWidth, shadowHeight);
    shadowMapping.Bind();
    {
      const Mat4f lightProjection   = Math::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, INITIAL_ZNEAR, INITIAL_ZFAR);
      const Mat4f lightView         = Math::LookAt(lightPosition, Vec3f(0.0f), Vec3f(0.0f, 1.0f, 0.0f));
      lightSpaceMatrix              = lightProjection * lightView;

      glClear(GL_DEPTH_BUFFER_BIT);
      shadowMapDepthShader.Use();
      shadowMapDepthShader.SetMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      scene.DrawScene(shadowMapDepthShader);
      
      /* TODO: resolve the problem of peter panning */
    }
    shadowMapping.Unbind();

    /* Render image to framebuffer */
    framebuffer.Bind();
    glViewport(0, 0, framebufferSize.x, framebufferSize.y);
    glClearColor(colorValue, colorValue, colorValue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Render scene with shadows */
    {
      shadowMapShader.Use();
      shadowMapShader.SetMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
      shadowMapShader.SetMat4f(SHADER_UNIFORM_VIEW,       cameraViewMatrix);
      shadowMapShader.SetMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      shadowMapShader.SetVec3f(SHADER_UNIFORM_LIGHT_POS,  lightPosition);
      shadowMapShader.SetVec3f(SHADER_UNIFORM_VIEW_POS,   camera.cameraComponent->position);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, depthMapTexture);
      scene.DrawScene(shadowMapShader);
    }

    /* Render depth map for visual debugging */
    {
      //testingShader.Use();
      //testingShader.SetFloat("near_plane", INITIAL_ZNEAR);
      //testingShader.SetFloat("far_plane", INITIAL_ZFAR);
      //testingShader.SetInt("depthMap", 0);
      //glActiveTexture(GL_TEXTURE0);
      //glBindTexture(GL_TEXTURE_2D, depthMapTexture);
      //Renderer::drawMode = GL_TRIANGLE_STRIP;
      //Renderer::DrawArrays(quadVao);
      //Renderer::drawMode = GL_TRIANGLES;
    }

    framebuffer.Blit();
    framebuffer.Unbind();

    editor.Render(scene, camera, framebufferImage);
    editor.End();

    /* -------------------------- Resizing framebuffer -------------------------- */
    const Vec2i viewportSize = editor.viewportPanel->viewportSize;
    if (framebufferSize != viewportSize)
    {
      framebuffer.Rescale(viewportSize);
      camera.cameraComponent->aspect = framebuffer.GetAspect();
      camera.cameraComponent->UpdateProjection();
    }

    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
#endif
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

void Engine::SetGLStates()
{
  /* Depth test ON */
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);   /* enable/disable writing into the depth buffer */
  glDepthFunc(GL_LESS);   /* specify the value used for depth buffer comparisons */

  /* Stencil test OFF */
  glDisable(GL_STENCIL_TEST);
  /* 
    only describes whether OpenGL should pass or discard fragments based on the stencil buffer's content, 
    not how we can actually update the buffer 
  */
  glStencilFunc(GL_EQUAL, 0, 0xFF); 
  /*
    whatever the outcome of any of the tests,the stencil buffer keeps its values. 
    The default behavior does not update the stencil buffer, so if you want to write to the
    stencil buffer you need to specify at least one different action for any of the options
  */
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 
  //glStencilMask(0xFF);  /* enable write to stencil buffer */
  //glStencilMask(0x00);  /* disable write to stencil buffer */

             
  /* Blending OFF */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Culling OFF */
  glDisable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  /* Gamma correction OFF */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  glEnable(GL_MULTISAMPLE);
}

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

  //framebufferShader.Use();
  //framebufferShader.SetInt("UScreenTexture", 0);
  //framebufferShader.SetInt("UPostProcessingType", 0);

  //instancingShader.Use();
  //instancingShader.SetInt("UMaterial.diffuse", 0);
  //instancingShader.SetInt("UMaterial.specular", 1);
  //instancingShader.SetFloat("UMaterial.shininess", 32.0f);
  //instancingShader.SetFloat(SHADER_UNIFORM_GAMMA, GAMMA_CORRECTION);

  //sceneShader.Use();
  //sceneShader.SetInt("UMaterial.diffuse", 0);
  //sceneShader.SetInt("UMaterial.specular", 1);
  //sceneShader.SetFloat("UMaterial.shininess", 32.0f);
  //sceneShader.SetFloat(SHADER_UNIFORM_GAMMA, GAMMA_CORRECTION);
  
  shadowMapShader.Use();
  shadowMapShader.SetInt(SHADER_UNIFORM_MATERIAL_DIFFUSE, 0);
  shadowMapShader.SetInt(SHADER_UNIFORM_MATERIAL_SPECULAR, 1);
  shadowMapShader.SetInt(SHADER_UNIFORM_SHADOW_MAP, 2);
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
