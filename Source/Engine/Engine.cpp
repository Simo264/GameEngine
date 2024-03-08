#include "Engine.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/FileParser/INIFileParser.hpp"
#include "Core/Platform/OpenGL/OpenGL.hpp"

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

#include <entt/entt.hpp> /* Entity component system */

Mat4f cameraProjectionMatrix;
Mat4f cameraViewMatrix;


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
  auto& instanceSM            = ShaderManager::Instance();
  auto& testingShader         = instanceSM.GetShader("TestingShader");
  auto& instancingShader      = instanceSM.GetShader("InstancingShader");
  auto& framebufferShader     = instanceSM.GetShader("FramebufferShader");
  auto& sceneShader           = instanceSM.GetShader("SceneShader");
  auto& shadowMapDepthShader  = instanceSM.GetShader("ShadowMapDepthShader");
  auto& shadowMapShader       = instanceSM.GetShader("ShadowMapShader");

  /* Initialize framebuffer object */
  auto& window = WindowManager::Instance();
  Vec2i framebufferSize;
  window.GetFramebufferSize(framebufferSize.x, framebufferSize.y);

  FrameBuffer framebuffer(framebufferSize.x, framebufferSize.y);
  glViewport(0, 0, framebufferSize.x, framebufferSize.y);

  /* Camera object */
  Camera camera(Vec3f(0.0f, 1.0f, 10.0f), 45.0f);
  
  /* Create scene */
  Scene scene;
  scene.LoadScene(ROOT_PATH / "scene.ini");


#if 0
  /* Shadow mapping */
  uint32_t SHADOW_WIDTH = 1024;
  uint32_t SHADOW_HEIGHT = 1024;
  uint32_t depthMapFBO; /* Framebuffer object for rendering the depth map */
  uint32_t depthMap;    /* Create a 2D texture that we'll use as the framebuffer's depth buffer */
  {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); /* Attach 2D texture as the framebuffer's depth buffer*/
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  VertexArray screenQuad;
  {
    float vertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    VertexArrayData data;
    data.vertDataSize = sizeof(vertices);
    data.vertData = vertices;
    VertexArrayConfig config;
    config.PushAttributes({ 3, 2 });

    screenQuad.InitVertexArray(data, config);
  }

  testingShader->Use();
  testingShader->SetInt("UShadowMap", 0);
  shadowMapShader->Use();
  shadowMapShader->SetInt("UDiffuseTexture", 0);
  shadowMapShader->SetInt("USpecularTexture", 1);
  shadowMapShader->SetInt("UShadowMap", 2);
#endif

  /* Pre-loop */
  TimePoint lastUpdateTime = SystemClock::now();
  const Vec2f perspectiveDistance(0.1f, 100.0f); 
  //Vec3f lightPos = Vec3f(-2.0f, 10.0f, -2.0f); /* Shadows */
  //Mat4f lightSpaceMatrix; /* Shadows */

  /* Loop  */
  while (window.IsOpen())
  {
    editor.Begin();

    /* Per-frame time logic */
    const auto now = SystemClock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    Renderer::drawCalls = 0;

    /* Input */
    window.PoolEvents();
    if (editor.viewportPanel->isFocused)
      camera.ProcessInput(delta);
    
    const Vec2i framebufferSize = framebuffer.GetSize();
    const float aspectRatio = ((float)framebufferSize.x / (float)framebufferSize.y);
    cameraProjectionMatrix = Math::Perspective(Math::Radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    cameraViewMatrix = camera.GetViewMatrix();
    
    /* Render: bind frame buffer */
    framebuffer.BindMSAAFramebuffer();
    glClearColor(pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    sceneShader.Use();
    sceneShader.SetMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
    sceneShader.SetMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
    scene.DrawScene(sceneShader);
    
    framebuffer.BlitFrameBuffer();
    framebuffer.UnbindFrameBuffer();

#if 0
    /* Fill the depth map framebuffer (from light's perspective) */
    {
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      Mat4f lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
      Mat4f lightView = glm::lookAt(lightPos, Vec3f(0.0f), Vec3f(0.0f, 1.0f, 0.0f));
      lightSpaceMatrix = lightProjection * lightView;
      shadowMapDepthShader->Use();
      shadowMapDepthShader->SetMat4f("ULightSpaceMatrix", lightSpaceMatrix);
      plane.Draw(shadowMapDepthShader);
      glCullFace(GL_FRONT); /* To fix peter panning we cull all front faces during the shadow map generation */
      cube1.Draw(shadowMapDepthShader);
      cube2.Draw(shadowMapDepthShader);
      glCullFace(GL_FRONT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      framebuffer.BindMSAAFramebuffer();
    }


    /* Reset viewport */
    glViewport(0, 0, windowFbSize.x, windowFbSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render scene as normal using the generated shadow map */
    {

      shadowMapShader->Use();
      shadowMapShader->SetMat4f("UView", view);
      shadowMapShader->SetMat4f("UProjection", projection);
      shadowMapShader->SetMat4f("ULightSpaceMatrix", lightSpaceMatrix);
      shadowMapShader->SetVec3f("ULightPos", lightPos);
      shadowMapShader->SetVec3f("UViewPos", camera.position);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      plane.Draw(shadowMapShader);
      cube1.Draw(shadowMapShader);
      cube2.Draw(shadowMapShader);
    }

    /* Draw frame buffer image to editor Viewport */
    framebuffer.BlitFrameBuffer();
    framebuffer.UnbindFrameBuffer();

    const double time = glfwGetTime();
    lightPos.x = sin(time) * 10.f;
    lightPos.z = cos(time) * 10.f;

    /* Render Depth map to quad for visual debugging */
    {
      testingShader->Use();
      testingShader->SetFloat("UNearPlane", near_plane);
      testingShader->SetFloat("UFarPlane", far_plane);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      Renderer::drawMode = GL_TRIANGLE_STRIP;
      Renderer::DrawArrays(&screenQuad);
      Renderer::drawMode = GL_TRIANGLES;
    }
#endif

    /* Render editor */
    editor.Render(scene, framebuffer);
    editor.End();
    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
  
    /* Destroy framebuffer */
    framebuffer.DestroyFrameBuffer();
}

void Engine::CleanUp()
{
  /* Destroy scene ...*/

  /* Destroy framebuffer ... */
  
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
