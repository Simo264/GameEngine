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

#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/ShaderUniforms.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/ECS/GameObject.hpp"
#include "Engine/ECS/Components.hpp"

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
  auto& testingShader         = instanceSM.GetShader("TestingShader");
  auto& instancingShader      = instanceSM.GetShader("InstancingShader");
  auto& framebufferShader     = instanceSM.GetShader("FramebufferShader");
  auto& sceneShader           = instanceSM.GetShader("SceneShader");
  auto& shadowMapDepthShader  = instanceSM.GetShader("ShadowMapDepthShader");
  auto& shadowMapShader       = instanceSM.GetShader("ShadowMapShader");

  float positions[] = {
     0.5f, -0.5f, 0.0f,  
    -0.5f, -0.5f, 0.0f,  
     0.0f,  0.5f, 0.0f,  
  };
  uint8_t colors[] = {
    255, 0, 0,
    0, 255, 0,
    0, 0, 255,
  };

  /* create vertex buffer with positions */
  VertexBuffer vbo_pos(sizeof(positions), positions, GL_STATIC_DRAW);

  /* create vertex buffer with colors */
  VertexBuffer vbo_col(sizeof(colors), colors, GL_STATIC_DRAW);

  /* create vertex array, bind vertex buffer, set attribute format, enable attribute */
  VertexArray vao;
  vao.Create();

  const int bindingPosition = 0;  /* binding point 0 -> position */
  const int bindingColor    = 1;  /* binding point 1 -> color */

  const int attrPosIndex    = 10; /* layout=10 -> position */
  const int attrColIndex    = 11; /* laytou=11 -> color */

  /* position -> 3 floats */
  /* color    -> 3 unsigned bytes */
  vao.SetAttribFormat(attrPosIndex, 3, GL_FLOAT, true, 0);
  vao.SetAttribFormat(attrColIndex, 3, GL_UNSIGNED_BYTE, true, 0);

  /* vbo_pos -> binding point 0 */
  /* vbo_col -> binding point 1 */
  vao.BindVertexBuffer(bindingPosition, vbo_pos, 0, 3 * sizeof(float));
  vao.BindVertexBuffer(bindingColor,    vbo_col, 0, 3 * sizeof(uint8_t));
  
  /* set binding position: 10 -> 0*/
  /* set binding color:    11 -> 1*/
  vao.SetAttribBinding(attrPosIndex, bindingPosition);
  vao.SetAttribBinding(attrColIndex, bindingColor);

  vao.EnableAttribute(attrPosIndex);
  vao.EnableAttribute(attrColIndex);


  while (window.IsOpen())
  {
    window.PoolEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    testingShader.Use();
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);

    window.SwapWindowBuffers();
  }

#if 0


  /* -------------------------- Framebuffer -------------------------- */
  Vec2i framebufferSize = window.GetFramebufferSize();
  //Vec2i framebufferSize = { 957, 498 };
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

  float quadVertices[] = {
    // positions        // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  };

  //VertexBufferLayout layout;
  //layout.PushAttributes({ 3,2 });
  //VertexBufferData data;
  //data.vertexDataSize = sizeof(quadVertices);
  //data.vertextDataPtr = quadVertices;
  //VertexArray quadVao(layout, data, GL_STATIC_DRAW);

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

    /* Render scene as normal */
    {
      //glViewport(0, 0, framebufferSize.x, framebufferSize.y);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      //sceneShader.Use();
      //sceneShader.SetMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
      //sceneShader.SetMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
      //scene.DrawScene(sceneShader);
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
  /* Depth test OFF */
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);   /* enable or disable writing into the depth buffer */
  glDepthFunc(GL_LESS);   /* specify the value used for depth buffer comparisons */

  /* Stencil test OFF */
  glDisable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 0, 0xFF); /* glStencilFunc only describes whether OpenGL should pass or discard
                                       fragments based on the stencil buffer's content, not how we can actually
                                       update the buffer */

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); /* whatever the outcome of any of the tests,
                                             the stencil buffer keeps its values. The default behavior does
                                             not update the stencil buffer, so if you want to write to the
                                             stencil buffer you need to specify at least one different action
                                             for any of the options */

                                             //glStencilMask(0xFF);  /* enable write to stencil buffer */
                                             //glStencilMask(0x00);  /* disable write to stencil buffer */

                                             /* Blending OFF */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Culling OFF */
  glDisable(GL_CULL_FACE);
  glCullFace(GL_FRONT); /* specify whether front- or back-facing facets can be culled */
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
