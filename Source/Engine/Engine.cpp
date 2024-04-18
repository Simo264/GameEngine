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
  
  Program& shadowMapDepthProg =
    instanceSM.LoadProgram("ShadowMapDepth", 
      *instanceSM.GetShader("ShadowMapDepth.vert"), 
      *instanceSM.GetShader("ShadowMapDepth.frag"));
  
  Program& shadowMapProg = instanceSM.LoadProgram(
    "ShadowMap", 
    *instanceSM.GetShader("ShadowMap.vert"), 
    *instanceSM.GetShader("ShadowMap.frag"));

  frameBufferProg.SetUniform1i(frameBufferProg.GetUniformLocation("UScreenTexture"), 0);
  frameBufferProg.SetUniform1i(frameBufferProg.GetUniformLocation("UPostProcessingType"), 0);

  instancingProg.SetUniform1i(instancingProg.GetUniformLocation("UMaterial.diffuse"), 0);
  instancingProg.SetUniform1i(instancingProg.GetUniformLocation("UMaterial.specular"), 1);
  instancingProg.SetUniform1f(instancingProg.GetUniformLocation("UMaterial.shininess"), 32.0f);
  instancingProg.SetUniform1f(instancingProg.GetUniformLocation(SHADER_UNIFORM_GAMMA), GAMMA_CORRECTION);

  sceneProg.SetUniform1i(sceneProg.GetUniformLocation("UMaterial.diffuse"), 0);
  sceneProg.SetUniform1i(sceneProg.GetUniformLocation("UMaterial.specular"), 1);
  sceneProg.SetUniform1f(sceneProg.GetUniformLocation("UMaterial.shininess"), 32.0f);
  sceneProg.SetUniform1f(sceneProg.GetUniformLocation(SHADER_UNIFORM_GAMMA), GAMMA_CORRECTION);

  shadowMapProg.SetUniform1i(shadowMapProg.GetUniformLocation(SHADER_UNIFORM_MATERIAL_DIFFUSE), 0);
  shadowMapProg.SetUniform1i(shadowMapProg.GetUniformLocation(SHADER_UNIFORM_MATERIAL_SPECULAR), 1);
  shadowMapProg.SetUniform1i(shadowMapProg.GetUniformLocation(SHADER_UNIFORM_SHADOW_MAP), 2);
}

static void CreateCube(VertexArray& vao)
{
  float vertices[] = {
      // position           // text coord
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
  color_att.Create(GL_TEXTURE_2D);
  color_att.CreateStorage(1600, 900);
  color_att.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  color_att.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  RenderBuffer depth_stenc_att;
  depth_stenc_att.internalformat = GL_DEPTH24_STENCIL8;
  depth_stenc_att.Create();
  depth_stenc_att.CreateStorage(1600, 900);

  framebuffer.AttachTexture(GL_COLOR_ATTACHMENT0, color_att, 0);
  framebuffer.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depth_stenc_att);

  if(framebuffer.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Framebuffer is not complete!");
}
static void CreateFramebufferMultisampled(FrameBuffer& multisampled_framebuffer, FrameBuffer& intermediate_framebuffer)
{
  multisampled_framebuffer.Create();

  // create a multisampled color attachment texture
  Texture2D text_col_mult_att;
  text_col_mult_att.format = GL_RGB;
  text_col_mult_att.internalformat = GL_RGB8;
  text_col_mult_att.Create(GL_TEXTURE_2D_MULTISAMPLE);
  text_col_mult_att.CreateStorageMultisampled(4, 1600, 900);

  // create a renderbuffer object for depth and stencil attachments
  RenderBuffer depth_stenc_mult_att;
  depth_stenc_mult_att.internalformat = GL_DEPTH24_STENCIL8;
  depth_stenc_mult_att.Create();
  depth_stenc_mult_att.CreateStorageMulstisampled(4, 1600, 900);

  multisampled_framebuffer.AttachTexture(GL_COLOR_ATTACHMENT0, text_col_mult_att, 0);
  multisampled_framebuffer.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depth_stenc_mult_att);

  if (multisampled_framebuffer.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");


  // configure second post-processing framebuffer
  intermediate_framebuffer.Create();

  // create a color attachment texture
  Texture2D text_col_att;
  text_col_att.format = GL_RGB;
  text_col_att.internalformat = GL_RGB8;

  text_col_att.Create(GL_TEXTURE_2D);
  text_col_att.CreateStorage(1600, 900);
  text_col_att.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  text_col_att.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  intermediate_framebuffer.AttachTexture(GL_COLOR_ATTACHMENT0, text_col_att, 0);

  if (intermediate_framebuffer.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
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

#if 1
  Camera camera({ 0.0f, 0.0f, 5.0f }, 45.0f, (16.0f/9.0f));

  VertexArray cube;
  CreateCube(cube);
  VertexArray framebuffer_quad;
  CreateQuad(framebuffer_quad);

  FrameBuffer multisampled_framebuffer;
  FrameBuffer intermediate_framebuffer;
  CreateFramebufferMultisampled(multisampled_framebuffer, intermediate_framebuffer);

  Texture2D* texture = instanceTM.GetTextureByPath((TEXTURES_PATH / "container_diffuse.png").lexically_normal());

  time_point lastUpdateTime = system_clock::now();
  while (window.IsOpen())
  {
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();

    window.PoolEvents();
    camera.ProcessInput(window, delta);
    const auto& cameraViewMatrix        = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix  = camera.cameraComponent->GetProjection();
    
    /* Bind to framebuffer and draw scene as we normally would to color texture */
    //multisampled_framebuffer.Bind(GL_FRAMEBUFFER);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    testingProgram->Use();
    testingProgram->SetUniformMat4f(testingProgram->GetUniformLocation(SHADER_UNIFORM_PROJECTION), cameraProjectionMatrix);
    testingProgram->SetUniformMat4f(testingProgram->GetUniformLocation(SHADER_UNIFORM_VIEW), cameraViewMatrix);
    testingProgram->SetUniformMat4f(testingProgram->GetUniformLocation(SHADER_UNIFORM_MODEL), Mat4f(1.0f));
    testingProgram->SetUniform1i(testingProgram->GetUniformLocation("UTexture"), 0);

    texture->BindTextureUnit(0);
    Renderer::DrawArrays(cube);

    /* Now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO */
    //multisampled_framebuffer.Blit(intermediate_framebuffer, 0, 0, 1600, 900, 0, 0, 1600, 900, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    /* Now bind back to default framebuffer and draw the image */
    //multisampled_framebuffer.Unbind(GL_FRAMEBUFFER);
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    //framebufferShader.Use();
    //intermediate_framebuffer.GetTextureAttachment(0).BindTextureUnit(0);
    //Renderer::DrawArrays(framebuffer_quad);

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

  editor->CleanUp();
  delete editor;

  WindowManager::Instance().CleanUp();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


#if 0 
void Engine::LoadShaders()
{
  
}
#endif
