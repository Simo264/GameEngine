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
#include "Engine/ShaderUniforms.hpp"

#include "Engine/Graphics/VertexBuffer.hpp"
#include "Engine/Graphics/ElementBuffer.hpp"
#include "Engine/Graphics/RenderBuffer.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <GLFW/glfw3.h>

static void CreateCube(VertexArray& vao)
{
  float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };
  VertexBuffer vbo(sizeof(vertices), vertices, GL_STATIC_DRAW);

  vao.Create();
  {
    VertexSpecifications specs{};
    specs.attrindex = 0;
    specs.bindingindex = 0;
    specs.components = 3;
    specs.normalized = true;
    specs.relativeoffset = 0;
    specs.type = GL_FLOAT;
    vao.SetVertexSpecifications(specs);

    specs.attrindex = 1;
    specs.bindingindex = 0;
    specs.components = 2;
    specs.normalized = true;
    specs.relativeoffset = 3 * sizeof(float);
    specs.type = GL_FLOAT;
    vao.SetVertexSpecifications(specs);
  }
  vao.AttachVertexBuffer(0, vbo, 0, 5 * sizeof(float));
  vao.numVertices = sizeof(vertices) / (5 * sizeof(float));
  vao.numIndices = 0;
}

static void CreateQuad(VertexArray& vao)
{
  float vertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  VertexBuffer vbo(sizeof(vertices), vertices, GL_STATIC_DRAW);

  vao.Create();

  VertexSpecifications specs{};
  specs.attrindex = 0;
  specs.bindingindex = 0;
  specs.components = 2;
  specs.normalized = true;
  specs.relativeoffset = 0;
  specs.type = GL_FLOAT;
  vao.SetVertexSpecifications(specs);

  specs.attrindex = 1;
  specs.bindingindex = 0;
  specs.components = 2;
  specs.normalized = true;
  specs.relativeoffset = 2 * sizeof(float);
  specs.type = GL_FLOAT;
  vao.SetVertexSpecifications(specs);

  vao.AttachVertexBuffer(0, vbo, 0, 4 * sizeof(float));

  vao.numVertices = 6;
  vao.numIndices = 0;
}

static void CreateFramebuffer(FrameBuffer& framebuffer)
{
  framebuffer.Create();

  Texture2D color_att;
  color_att.format = GL_RGB;
  color_att.internalformat = GL_RGB8;
  color_att.Create();
  color_att.CreateStorage(1600, 900);
  color_att.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  color_att.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  RenderBuffer depth_stenc_att;
  depth_stenc_att.Create();
  depth_stenc_att.CreateStorage(GL_DEPTH24_STENCIL8, 1600, 900);

  framebuffer.AttachTexture(GL_COLOR_ATTACHMENT0, color_att, 0);
  framebuffer.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depth_stenc_att);

  if(!framebuffer.CheckStatus())
    CONSOLE_WARN("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

  //glCreateFramebuffers(1, &framebuffer_id);

  //// create a color attachment texture
  //glCreateTextures(GL_TEXTURE_2D, 1, &color_text_att);
  //glTextureStorage2D(color_text_att, 1, GL_RGB8, 1600, 900);
  //glTextureParameteri(color_text_att, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTextureParameteri(color_text_att, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  //glCreateRenderbuffers(1, &stenc_depth_rbo_att);
  //glNamedRenderbufferStorage(stenc_depth_rbo_att, GL_DEPTH24_STENCIL8, 1600, 900);

  //// attach color texture to framebuffer
  //glNamedFramebufferTexture(framebuffer_id, GL_COLOR_ATTACHMENT0, color_text_att, 0);
  //// attach renderbuffer to framebuffer object
  //glNamedFramebufferRenderbuffer(framebuffer_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stenc_depth_rbo_att);

  //if (glCheckNamedFramebufferStatus(framebuffer_id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  //  CONSOLE_WARN("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}

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
  TextureManager::Instance().LoadTextures();
  TextureManager::Instance().LoadIcons();

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

#if 1
  Camera camera({ 0.0f, 0.0f, 5.0f }, 45.0f, (16.0f/9.0f));
  
  Texture2D* texture = instanceTM.GetTextureByPath((TEXTURES_PATH / "container_diffuse.png"));

  VertexArray cube_vao;
  CreateCube(cube_vao);

  VertexArray quad_vao;
  CreateQuad(quad_vao);

  FrameBuffer framebuffer;
  CreateFramebuffer(framebuffer);

  //uint32_t framebuffer_id;
  //uint32_t color_text_att;
  //uint32_t sten_depth_rbo_att;
  //SetupFramebuffer(framebuffer_id, color_text_att, sten_depth_rbo_att);



  TimePoint lastUpdateTime = SystemClock::now();
  while (window.IsOpen())
  {
    const auto now = SystemClock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();

    window.PoolEvents();
    camera.ProcessInput(window, delta);
    const auto& cameraViewMatrix        = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix  = camera.cameraComponent->GetProjection();
    
    glViewport(0, 0, 1600, 900);

    framebuffer.Bind(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    testingShader.Use();
    testingShader.SetMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
    testingShader.SetMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
    testingShader.SetMat4f(SHADER_UNIFORM_MODEL, Mat4f(1.0f));
    testingShader.SetInt("UTexture", 0);

    texture->BindTextureUnit(0);
    Renderer::DrawArrays(cube_vao);

    framebuffer.Unbind(GL_FRAMEBUFFER);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    framebufferShader.Use();
    
    framebuffer.GetTextureAttachment(0).BindTextureUnit(0);
    Renderer::DrawArrays(quad_vao);

    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
#endif


#if 0
  /* -------------------------- Framebuffer -------------------------- */
  Vec2i32 framebufferSize = window.GetFramebufferSize();
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
      
      glBindTextureUnit(0, depthMapTexture);
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
    const Vec2i32 viewportSize = editor.viewportPanel->viewportSize;
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
  Vec2i32 resolution = INIFileParser::StringToVec2i(tmp, "x");
  tmp = conf.GetValue("window", "aspectratio");
  Vec2i32 aspectratio = INIFileParser::StringToVec2i(tmp, ":");
  tmp = conf.GetValue("window", "position");
  Vec2i32 position = INIFileParser::StringToVec2i(tmp, ",");
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

