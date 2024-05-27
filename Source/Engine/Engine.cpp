#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"

#include "Engine/Graphics/Depth.hpp"
#include "Engine/Graphics/Stencil.hpp"
#include "Engine/Graphics/Culling.hpp"

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

#include "GUI/Gui.hpp"

#define GUI_MODE 0

uint32_t drawCalls = 0;

static void SetGlobalGlStates()
{
  /* Depth testing */
  Depth::EnableTest();
  Depth::EnableWritingBuffer();
  Depth::SetFunction(GL_LESS);

  /* Stencil testing */
  Stencil::DisableTest();
  Stencil::SetFunction(GL_ALWAYS, 0, 0xFF);
  Stencil::SetOperation(GL_KEEP, GL_KEEP, GL_KEEP);

  /* Blending OFF */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Culling OFF */
  Culling::DisableFaceCulling();
  Culling::SetCullFace(GL_BACK);
  Culling::SetFrontFace(GL_CCW);

  /* Gamma correction OFF */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  glEnable(GL_MULTISAMPLE);

  glClearColor(0.15, 0.15, 0.15, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);
}
static void LoadConfig()
{
  auto& window = WindowManager::Instance();
  window.SetWindowTitle("ProjectGL");
  window.SetWindowSize(1600, 900);
  window.SetWindowPosition(50,50);
  window.SetWindowAspectRatio(16, 9);
  window.SetWindowVsync(false);
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
}
static void SetShadersUniforms()
{
  ShaderManager& instanceSM = ShaderManager::Instance();
  
  Program* frameBufferProg = instanceSM.GetProgram("Framebuffer");
  frameBufferProg->SetUniform1i("u_screenTexture", 0);
  frameBufferProg->SetUniform1i("u_postProcessingType", 0);
  
  Program* sceneProgram = instanceSM.GetProgram("Scene");
  sceneProgram->SetUniform1i("u_material.diffuse", 0);
  sceneProgram->SetUniform1i("u_material.specular", 1);
  sceneProgram->SetUniform1f("u_material.shininess", 32.0f);
  sceneProgram->SetUniform1f("u_gamma", 2.2f);
  
  Program* shadowMapProgram = instanceSM.GetProgram("ShadowMap");
  shadowMapProgram->SetUniform1i("u_material.diffuse", 0);
  shadowMapProgram->SetUniform1i("u_material.specular", 1);
  shadowMapProgram->SetUniform1f("u_material.shininess", 32.0f);
  shadowMapProgram->SetUniform1i("u_shadowMap", 10);
  shadowMapProgram->SetUniform1f("u_gamma", 2.2f);
}
static void CreateScreenSquare(VertexArray& vao)
{
  float vertices[] = {
    // positions   texCoords
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
static void CreateFramebufferMultisampled(FrameBuffer& fboMultisampled,FrameBuffer& fboIntermediate,int samples, int width, int height)
{
  fboMultisampled.Create();

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

  fboMultisampled.AttachTexture(GL_COLOR_ATTACHMENT0, textColMultAtt, 0);
  fboMultisampled.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthStencMultAtt);

  if (fboMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  /* Configure second post - processing framebuffer */
  fboIntermediate.Create();

  /* Create a color attachment texture */
  Texture2D textColAtt;
  textColAtt.format = GL_RGB;
  textColAtt.internalformat = GL_RGB8;
  textColAtt.Create(GL_TEXTURE_2D);
  textColAtt.CreateStorage(width, height);
  textColAtt.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  textColAtt.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  fboIntermediate.AttachTexture(GL_COLOR_ATTACHMENT0, textColAtt, 0);

  if (fboIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
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


  /* Resolve the problem of over sampling */
  depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  depthMap.SetParameterfv(GL_TEXTURE_BORDER_COLOR, array<float,4>{ 1.0, 1.0, 1.0, 1.0 }.data());
  
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

  ShaderManager::Instance().Initialize();
  LoadPrograms();
  
  TextureManager::Instance().Initialize();

  SetGlobalGlStates();
}

void Engine::Run() const
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
  SetShadersUniforms();

  VertexArray screenSquare;
  CreateScreenSquare(screenSquare);

  /* -------------------------- Framebuffer -------------------------- */
  vec2i32 viewportSize = window.GetFramebufferSize();
  FrameBuffer framebufferMultisampled;
  FrameBuffer framebufferIntermediate;
  CreateFramebufferMultisampled(framebufferMultisampled, framebufferIntermediate, 4, viewportSize.x, viewportSize.y);
  const Texture2D& framebufferTexture = framebufferIntermediate.GetTextureAttachment(0);

  /* -------------------------- Camera -------------------------- */
  Camera camera(vec3f(30.0f, 15.0f, 10.0f), 45.0f, static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y));
  camera.cameraComponent->yaw = -180.0f;
  camera.cameraComponent->pitch = -30.0f;
  camera.cameraComponent->roll = 0.0f;
  camera.cameraComponent->UpdateVectors();

  /* -------------------------- Scene -------------------------- */
  Scene scene;
  scene.LoadScene((ROOT_PATH / "Scene.ini"));

  /* -------------------------- Shadow mapping -------------------------- */
  //FrameBuffer framebufferShadowMap;
  //CreateShadowMapFramebuffer(framebufferShadowMap, 1024, 1024);
  //const Texture2D& DepthMapTexture = framebufferShadowMap.GetTextureAttachment(0);

  /* -------------------------- Pre-loop -------------------------- */
  time_point lastUpdateTime = system_clock::now();

  unique_pointer<Gui> gui;
  gui->SetupContext();

  
  /* -------------------------- loop -------------------------- */
  while (window.IsOpen())
  {
    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    gui->StartFrame();
    drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    window.PoolEvents();
    camera.ProcessInput(window, delta);


    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    const auto time = glfwGetTime();


    //const Mat4f lightProjection = Math::Ortho(
    //  -10.0f, 10.0f, -10.0f, 10.0f, INITIAL_ZNEAR, INITIAL_ZFAR);
    //Mat4f lightView = Math::LookAt(
    //  lightPosition,
    //  vec3f(0.0f, 0.0f, 0.0f), // vec3f(0.0f, 0.0f, 0.0f) | dirLight->direction 
    //  vec3f(0.0f, 1.0f, 0.0f)
    //);
    //Mat4f lightSpaceMatrix = lightProjection * lightView;

    /* -------------------------- Rendering -------------------------- */
    /* Render depth of scene to texture(from directional light's perspective) */
    //framebufferShadowMap.Bind(GL_FRAMEBUFFER);
    //{
    //  glViewport(0, 0, 1024, 1024);
    //  glClear(GL_DEPTH_BUFFER_BIT);
    //  shadowMapDepthProgram->Use();
    //  shadowMapDepthProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
    //  scene.DrawScene(*shadowMapDepthProgram);
    //}
    //framebufferShadowMap.Unbind(GL_FRAMEBUFFER);

    /* Fill the framebuffer color texture */
    framebufferMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, viewportSize.x, viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      /* Render scene without shadows map */
      {
        sceneProgram->Use();
        sceneProgram->SetUniformMat4f("u_projection", cameraProjectionMatrix);
        sceneProgram->SetUniformMat4f("u_view", cameraViewMatrix);
        sceneProgram->SetUniform3f("u_viewPos", camera.cameraComponent->position);
        sceneProgram->SetUniform1i("u_material.diffuseTexture", 0);
        sceneProgram->SetUniform1i("u_material.specularTexture", 1);
        sceneProgram->SetUniform1f("u_material.shininess", 32.0f);

        scene.Reg().view<DirLightComponent>().each([sceneProgram](auto& light) {
          sceneProgram->SetUniform3f("u_directionalLight.color",    light.color);
          sceneProgram->SetUniform1f("u_directionalLight.ambient",  light.ambient);
          sceneProgram->SetUniform1f("u_directionalLight.diffuse",  light.diffuse);
          sceneProgram->SetUniform1f("u_directionalLight.specular", light.specular);
          sceneProgram->SetUniform3f("u_directionalLight.direction",light.direction);
        });
        int i = 0;
        scene.Reg().view<PointLightComponent>().each([sceneProgram, &i](auto& light) {
          sceneProgram->SetUniform3f(std::format("u_pointLight[{}].color", i).c_str(),    light.color);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].ambient", i).c_str(),  light.ambient);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].diffuse", i).c_str(),  light.diffuse);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].specular", i).c_str(), light.specular);
          sceneProgram->SetUniform3f(std::format("u_pointLight[{}].position", i).c_str(), light.position);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].linear", i).c_str(),   light.linear);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].quadratic", i).c_str(),light.quadratic);
          i++;
        });
        scene.Reg().view<StaticMeshComponent, TransformComponent>().each([sceneProgram](auto& mesh, auto& transform) {
          sceneProgram->SetUniformMat4f("u_model", transform.GetTransformation());
          mesh.Draw();
        });
      }

      /* Render scene with shadows map */
      {
        //shadowMapProgram->Use();
        //shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_PROJECTION, cameraProjectionMatrix);
        //shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_VIEW, cameraViewMatrix);
        //shadowMapProgram->SetUniformMat4f(SHADER_UNIFORM_LIGHTSPACE, lightSpaceMatrix);
        //DepthMapTexture.BindTextureUnit(10);
        //scene.DrawScene(*shadowMapProgram);
      }

      /* Render Depth map texture for visual debugging */
      {
        //testingProgram->Use();
        //testingProgram->SetUniform1i("depthMap", 0);
        //testingProgram->SetUniform1f("near_plane", INITIAL_ZNEAR);
        //testingProgram->SetUniform1f("far_plane", INITIAL_ZFAR);
        //DepthMapTexture.BindTextureUnit(0);
        //DrawArrays(GL_TRIANGLES, screenSquare);
      }

      /* Blit multisampled buffer to normal colorbuffer of intermediate FBO */
      framebufferMultisampled.Blit(framebufferIntermediate,
        0, 0, viewportSize.x, viewportSize.y,
        0, 0, viewportSize.x, viewportSize.y,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
    }
    framebufferMultisampled.Unbind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebufferProgram->Use();
    framebufferTexture.BindTextureUnit(0);
    DrawArrays(GL_TRIANGLES, screenSquare);

    gui->Render();
    gui->EndFrame();

    /* -------------------------- Resizing framebuffer -------------------------- */
    const vec2i32 currentFramebufferSize = window.GetFramebufferSize();
    if (viewportSize != currentFramebufferSize)
    {
      viewportSize = currentFramebufferSize;
      framebufferMultisampled.Delete();
      framebufferIntermediate.Delete();
      CreateFramebufferMultisampled(framebufferMultisampled, framebufferIntermediate, 4, viewportSize.x, viewportSize.y);

      camera.cameraComponent->aspect = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);
      camera.cameraComponent->UpdateProjection();
    }

    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }

  gui->CleanUp();
}

void Engine::CleanUp() const
{
  ShaderManager::Instance().CleanUp();
  
  TextureManager::Instance().CleanUp();

  WindowManager::Instance().CleanUp();
}
