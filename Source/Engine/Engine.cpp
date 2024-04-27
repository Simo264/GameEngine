#include "Engine.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"
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

#include "Editor/Editor.hpp"

#include <GLFW/glfw3.h>

static void SetGLStates()
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

static void LoadConfig()
{
  INIFileParser conf(ROOT_PATH / CONFIG_FILENAME);
  conf.ReadData();

  string title = conf.GetValue("window", "title");

  string tmp;
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
static void LoadPrograms()
{
  ShaderManager& instanceSM = ShaderManager::Instance();
  Program& testingProg = instanceSM.LoadProgram(
    "Testing", 
    *instanceSM.GetShader("Testing.vert"), 
    *instanceSM.GetShader("Testing.frag"));

  Program& instancingProg = instanceSM.LoadProgram(
    "Instancing", 
    *instanceSM.GetShader("Instancing.vert"), 
    *instanceSM.GetShader("Scene.frag"));
  
  Program& frameBufferProg = instanceSM.LoadProgram(
    "Framebuffer", 
    *instanceSM.GetShader("Framebuffer.vert"),
    *instanceSM.GetShader("Framebuffer.frag"));
  
  Program& sceneProg = instanceSM.LoadProgram(
    "Scene", 
    *instanceSM.GetShader("Scene.vert"), 
    *instanceSM.GetShader("Scene.frag"));
  
  Program& shadowMapDepthProg = instanceSM.LoadProgram(
    "ShadowMapDepth", 
    *instanceSM.GetShader("ShadowMapDepth.vert"), 
    *instanceSM.GetShader("ShadowMapDepth.frag"));
  
  Program& shadowMapProg = instanceSM.LoadProgram(
    "ShadowMap", 
    *instanceSM.GetShader("ShadowMap.vert"), 
    *instanceSM.GetShader("ShadowMap.frag"));

  frameBufferProg.SetUniform1i(SHADER_UNIFORM_SCREEN_TEXTURE, 0);
  frameBufferProg.SetUniform1i(SHADER_UNIFORM_POST_PROCESSING, 0);

  instancingProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_DIFFUSE, 0);
  instancingProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_SPECULAR, 1);
  instancingProg.SetUniform1f(SHADER_UNIFORM_MATERIAL_SHININESS, 32.0f);
  instancingProg.SetUniform1f(SHADER_UNIFORM_GAMMA, GAMMA_CORRECTION);

  sceneProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_DIFFUSE, 0);
  sceneProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_SPECULAR, 1);
  sceneProg.SetUniform1f(SHADER_UNIFORM_MATERIAL_SHININESS, 32.0f);
  sceneProg.SetUniform1f(SHADER_UNIFORM_GAMMA, GAMMA_CORRECTION);

  shadowMapProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_DIFFUSE, 0);
  shadowMapProg.SetUniform1i(SHADER_UNIFORM_MATERIAL_SPECULAR, 1);
  shadowMapProg.SetUniform1i(SHADER_UNIFORM_SHADOW_MAP, 10);
}
static void CreateScreenSquare(VertexArray& vao)
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
static void CreateFramebufferMultisampled(
  FrameBuffer& framebufferMultisampled, 
  FrameBuffer& framebufferIntermediate, 
  int samples, 
  int width, 
  int height)
{
  framebufferMultisampled.Create();

  /* Create a multisampled color attachment texture */
  Texture2D textColMultAtt;
  textColMultAtt.format = GL_RGB;
  textColMultAtt.internalformat = GL_RGB8;
  textColMultAtt.Create(GL_TEXTURE_2D_MULTISAMPLE);
  textColMultAtt.CreateStorageMultisampled(samples, width, height);

  /* Create a renderbuffer object for depth and stencil attachments */
  RenderBuffer depthStencMultAtt;
  depthStencMultAtt.internalformat = GL_DEPTH24_STENCIL8;
  depthStencMultAtt.Create();
  depthStencMultAtt.CreateStorageMulstisampled(samples, width, height);

  framebufferMultisampled.AttachTexture(GL_COLOR_ATTACHMENT0, textColMultAtt, 0);
  framebufferMultisampled.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthStencMultAtt);

  if (framebufferMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  /* Configure second post - processing framebuffer */
  framebufferIntermediate.Create();

  /* Create a color attachment texture */
  Texture2D textColAtt;
  textColAtt.format = GL_RGB;
  textColAtt.internalformat = GL_RGB8;
  textColAtt.Create(GL_TEXTURE_2D);
  textColAtt.CreateStorage(width, height);
  textColAtt.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  textColAtt.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  framebufferIntermediate.AttachTexture(GL_COLOR_ATTACHMENT0, textColAtt, 0);

  if (framebufferIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
}

static void CreateShadowMapFramebuffer(FrameBuffer& framebuffer, int width, int height)
{
  framebuffer.Create();
   
  /*
    Create a 2D texture that we'll use as the framebuffer's depth buffer.
    We also give the texture a width and height: this is the resolution of the depth map
  */
  Texture2D depthMap;
  depthMap.internalformat = GL_DEPTH_COMPONENT16;
  depthMap.Create(GL_TEXTURE_2D);
  depthMap.CreateStorage(width, height);
  depthMap.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  depthMap.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);

  /* Resolve the problem of over sampling */
  //depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  //depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  /* With the generated depth texture we can attach it as the framebuffer's depth buffer */
  framebuffer.AttachTexture(GL_DEPTH_ATTACHMENT, depthMap, 0);
}

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
  LoadPrograms();
  
  TextureManager::Instance().Initialize();

  editor = new Editor;
  editor->Initialize();
}

void Engine::Run()
{
  WindowManager& window           = WindowManager::Instance();
  ShaderManager& instanceSM       = ShaderManager::Instance();
  TextureManager& instanceTM      = TextureManager::Instance();
  Program* testingProgram         = instanceSM.GetProgram("Testing");
  Program* instancingProgram      = instanceSM.GetProgram("Instancing");
  Program* framebufferProgram     = instanceSM.GetProgram("Framebuffer");
  Program* sceneProgram           = instanceSM.GetProgram("Scene");
  Program* shadowMapDepthProgram  = instanceSM.GetProgram("ShadowMapDepth");
  Program* shadowMapProgram       = instanceSM.GetProgram("ShadowMap");

  VertexArray screenSquare;
  CreateScreenSquare(screenSquare);

  /* -------------------------- Framebuffer -------------------------- */
  Vec2i32 viewportSize = window.GetFramebufferSize();
  FrameBuffer framebufferMultisampled;
  FrameBuffer framebufferIntermediate;
  CreateFramebufferMultisampled(framebufferMultisampled, framebufferIntermediate, 4, viewportSize.x, viewportSize.y);


  /* -------------------------- Camera -------------------------- */
  Camera camera(Vec3f(0.0f, 0.0f, 5.0f), 45.0f, static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y));


  /* -------------------------- Scene -------------------------- */
  Scene scene;
  scene.LoadScene(ROOT_PATH / "Scene.ini");


  /* -------------------------- Shadow mapping -------------------------- */
  FrameBuffer framebufferShadowMap;
  CreateShadowMapFramebuffer(framebufferShadowMap, 1024, 1024);

  const Vec3f lightPosition = { -2.0f, 4.0f, -1.0f };
  const Mat4f lightProjection = Math::Ortho(-10.0f, 10.0f, -10.0f, 10.0f, INITIAL_ZNEAR, INITIAL_ZFAR);


  /* -------------------------- Pre-loop -------------------------- */
  time_point lastUpdateTime = system_clock::now();
  const double colorValue = Math::Pow(static_cast<double>(0.3), static_cast<double>(GAMMA_CORRECTION));
  glClearColor(colorValue, colorValue, colorValue, 1.0f);
  glClearDepth(1.0f);

  const Texture2D& framebufferTexture = framebufferIntermediate.GetTextureAttachment(0);
  const Texture2D& DepthMapTexture    = framebufferShadowMap.GetTextureAttachment(0);
  

#if 0
  /* -------------------------- Loop -------------------------- */
  while (window.IsOpen())
  {
    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    Renderer::drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    window.PoolEvents();
    camera.ProcessInput(window, delta);


    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    Mat4f lightView = Math::LookAt(lightPosition, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
    Mat4f lightSpaceMatrix = lightProjection * lightView;
    
    /* -------------------------- Rendering -------------------------- */
    
    /* 1. Render depth of scene to texture(from directional light's perspective) */
    framebufferShadowMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      shadowMapDepthProgram->Use();
      shadowMapDepthProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      scene.DrawScene(*shadowMapDepthProgram);
    }
    framebufferShadowMap.Unbind(GL_FRAMEBUFFER);

    /* 2. Render scene with shadow map */
    framebufferMultisampled.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, framebufferSize.x, framebufferSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shadowMapProgram->Use();
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      DepthMapTexture.BindTextureUnit(10);
      scene.DrawScene(*shadowMapProgram);

      /* Blit multisampled buffer to normal colorbuffer of intermediate FBO */
      framebufferMultisampled.Blit(framebufferIntermediate,
        0, 0, framebufferSize.x, framebufferSize.y,
        0, 0, framebufferSize.x, framebufferSize.y,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    framebufferMultisampled.Unbind(GL_FRAMEBUFFER);

    /* 3. Render framebuffer image texture */
    framebufferProgram->Use();
    framebufferTexture.BindTextureUnit(0);
    Renderer::DrawArrays(screenSquare);


    /* Render Depth map texture for visual debugging */
    {
      //testingProgram->Use();
      //testingProgram->SetUniform1i("depthMap", 0);
      //testingProgram->SetUniform1f("near_plane", INITIAL_ZNEAR);
      //testingProgram->SetUniform1f("far_plane", INITIAL_ZFAR);
      //DepthMapTexture.BindTextureUnit(0);
      //Renderer::DrawArrays(screenSquare);
    }


    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
#endif


#if 1
  /* -------------------------- Loop -------------------------- */
  while (window.IsOpen())
  {
    editor->Begin();

    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    Renderer::drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    window.PoolEvents();
    if (editor->viewportPanel->isFocused)
      camera.ProcessInput(window, delta);

    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    Mat4f lightView = Math::LookAt(lightPosition, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
    Mat4f lightSpaceMatrix = lightProjection * lightView;

    /* -------------------------- Rendering -------------------------- */
    /* 1. Render depth of scene to texture(from directional light's perspective) */
    framebufferShadowMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      shadowMapDepthProgram->Use();
      shadowMapDepthProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      scene.DrawScene(*shadowMapDepthProgram);
    }
    framebufferShadowMap.Unbind(GL_FRAMEBUFFER);

    /* 2. Render scene with shadow map */
    framebufferMultisampled.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, viewportSize.x, viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shadowMapProgram->Use();
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
      shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
      DepthMapTexture.BindTextureUnit(10);
      scene.DrawScene(*shadowMapProgram);

      /* Blit multisampled buffer to normal colorbuffer of intermediate FBO */
      framebufferMultisampled.Blit(framebufferIntermediate,
        0, 0, viewportSize.x, viewportSize.y,
        0, 0, viewportSize.x, viewportSize.y,
        GL_COLOR_BUFFER_BIT, 
        GL_NEAREST);
    }
    framebufferMultisampled.Unbind(GL_FRAMEBUFFER);

    editor->Render(scene, camera, framebufferTexture.id);
    editor->End();

    /* -------------------------- Resizing framebuffer -------------------------- */
    const auto& viewportPanel = editor->viewportPanel;
    if (viewportSize != viewportPanel->viewportSize)
    {
      viewportSize = viewportPanel->viewportSize;
      framebufferMultisampled.Delete();
      framebufferIntermediate.Delete();
      CreateFramebufferMultisampled(framebufferMultisampled, framebufferIntermediate, 4, viewportSize.x, viewportSize.y);
      
      camera.cameraComponent->aspect = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);
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

  editor->CleanUp();
  delete editor;

  WindowManager::Instance().CleanUp();
}
