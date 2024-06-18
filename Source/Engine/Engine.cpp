#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Depth.hpp"
#include "Engine/Graphics/Stencil.hpp"
#include "Engine/Graphics/Culling.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Graphics/TextureCubemap.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/Globals.hpp"

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

uint32_t drawCalls = 0;

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  Logger::Initialize();
  CONSOLE_INFO("Logger initialized");

  /* Initialize window */
  _instanceWM = WindowManager::Instance();
  _instanceWM->Initialize();
  _instanceWM->SetWindowTitle("ProjectGL");
  _instanceWM->SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  _instanceWM->SetWindowPosition(50, 50);
  _instanceWM->SetWindowAspectRatio(16, 9);
  _instanceWM->SetWindowVsync(false);
  
  /* Setup ImGui context */
  ImGuiLayer::SetupContext();
  ImGuiLayer::SetFont((FONTS_PATH / "Karla-Regular.ttf"), 16);
  
  /* Initialize shaders */
  _instanceSM = ShaderManager::Instance();
  _instanceSM->LoadShadersFromDir(SHADERS_PATH);
  _instanceSM->LoadPrograms();
  _instanceSM->SetUpProgramsUniforms();

  /* Initialize textures */
  _instanceTM = TextureManager::Instance();
  LoadTextures();

  /* Create Framebuffer object */
  _viewport = _instanceWM->GetFramebufferSize();
  CreateFramebuffer(4, _viewport.x, _viewport.y);
  CreateScreenSquare();
  
  /* Create Framebuffer object for shadows */
  CreateFramebufferShadowMap(1024, 1024);

  /* Initialize uniform block objects */
  _uboCamera.target = GL_UNIFORM_BUFFER;
  _uboCamera.Create();
  _uboCamera.CreateStorage(2 * sizeof(mat4f), nullptr, GL_STATIC_DRAW);
  _uboCamera.BindBase(0); /* cameraBlock to bindingpoint 0 */

  SetOpenGLStates();
}
void Engine::Run()
{
  StaticMeshComponent mesh(ASSETS_PATH / "Shapes/Cube/Cube.obj");
  TransformComponent transform{};

  Camera camera(
    vec3f(0.0f, 0.0f, 5.0f),
    45.0f, 
    static_cast<float>(_viewport.x) / static_cast<float>(_viewport.y), 
    Z_NEAR, 
    Z_FAR
  );


  /* -------------------------- Pre-loop -------------------------- */
  Program* testingProgram = _instanceSM->GetProgram("Testing");
  Program* framebufferProgram = _instanceSM->GetProgram("Framebuffer");
  Texture2D& fboImageTexture = _fboIntermediate.GetTextureAttachment(0);
  
  Texture2D* brickwallTexture = _instanceTM->GetTextureByPath(TEXTURES_PATH / "brickwall.jpg");
  Texture2D* brickwallNormalTexture = _instanceTM->GetTextureByPath(TEXTURES_PATH / "brickwall_normal.jpg");
  testingProgram->SetUniform1i("diffuseTexture", 0);
  testingProgram->SetUniform1i("normalTexture", 2);
  
  time_point lastUpdateTime = system_clock::now();
  
  /* -------------------------- loop -------------------------- */
  while (_instanceWM->IsOpen())
  {
    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    const float time = glfwGetTime();
    drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    _instanceWM->PoolEvents();
    camera.ProcessInput(delta);


    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    _uboCamera.UpdateStorage(0, sizeof(mat4f), &cameraViewMatrix[0]);
    _uboCamera.UpdateStorage(sizeof(mat4f), sizeof(mat4f), &cameraProjectionMatrix[0]);

    
    /* -------------------------- Rendering -------------------------- */

    /* Fill the framebuffer color texture */
    _fboMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, _viewport.x, _viewport.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      testingProgram->Use();
      testingProgram->SetUniform3f("viewPos", camera.cameraComponent->position);
      testingProgram->SetUniformMat4f("u_model", transform.GetTransformation());
      mesh.Draw();


      /* Blit multisampled buffer to normal colorbuffer of intermediate FBO */
      _fboMultisampled.Blit(_fboIntermediate,
        0, 0, _viewport.x, _viewport.y,
        0, 0, _viewport.x, _viewport.y,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
    }
    _fboMultisampled.Unbind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebufferProgram->Use();
    fboImageTexture.BindTextureUnit(0);
    Renderer::DrawArrays(GL_TRIANGLES, _screenSquare);

    /* -------------------------- Resizing framebuffer -------------------------- */
    const vec2i32 currentFramebufferSize = _instanceWM->GetFramebufferSize();
    if (_viewport != currentFramebufferSize)
    {
      ResizeFramebuffer(currentFramebufferSize);
      camera.cameraComponent->aspect = static_cast<float>(_viewport.x) / static_cast<float>(_viewport.y);
      camera.cameraComponent->UpdateProjection();
    }

    _instanceWM->SwapWindowBuffers();
    lastUpdateTime = now;
  }
}
void Engine::CleanUp()
{
  _fboIntermediate.Delete();
  _fboMultisampled.Delete();
  _screenSquare.Delete();
  _uboCamera.Delete();

  ImGuiLayer::CleanUp();

  _instanceSM->CleanUp();
  _instanceTM->CleanUp();
  _instanceWM->CleanUp();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Engine::SetOpenGLStates()
{
  /* Depth testing ON */
  Depth::EnableTest();
  Depth::EnableWritingBuffer();
  Depth::SetFunction(GL_LESS);

  /* Stencil testing OFF */
  Stencil::DisableTest();
  Stencil::SetFunction(GL_ALWAYS, 0, 0xFF);
  Stencil::SetOperation(GL_KEEP, GL_KEEP, GL_KEEP);

  /* Culling OFF */
  Culling::DisableFaceCulling();
  Culling::SetCullFace(GL_BACK);
  Culling::SetFrontFace(GL_CCW);

  /* Blending OFF */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Gamma correction OFF */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  glEnable(GL_MULTISAMPLE);

  glClearColor(0.15, 0.15, 0.15, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);
}
void Engine::LoadTextures() const
{
  /* Load textures */
  for (auto& entry : std::filesystem::recursive_directory_iterator(TEXTURES_PATH))
  {
    if (!std::filesystem::is_directory(entry))
    {
      const auto path = entry.path().lexically_normal();
      _instanceTM->LoadTexture(path);
    }
  }

  /* Load icons */
  for (auto& entry : std::filesystem::recursive_directory_iterator(ICONS_PATH))
  {
    if (!std::filesystem::is_directory(entry))
    {
      const auto path = entry.path().lexically_normal();
      _instanceTM->LoadTextureIcon(path);
    }
  }
}
void Engine::CreateFramebuffer(int samples, int width, int height)
{
  _fboMultisampled.Create();

  /* Create a multisampled color attachment texture */
  Texture2D textColMultAtt;
  textColMultAtt.format = GL_RGB;
  textColMultAtt.internalformat = GL_RGB8;
  textColMultAtt.Create(GL_TEXTURE_2D_MULTISAMPLE);
  textColMultAtt.CreateStorageMultisampled(samples, width, height);

  /* Create a multisampled renderbuffer object for depth and stencil attachments */
  RenderBuffer depthStencMultAtt;
  depthStencMultAtt.internalformat = GL_DEPTH24_STENCIL8;
  depthStencMultAtt.Create();
  depthStencMultAtt.CreateStorageMulstisampled(samples, width, height);

  _fboMultisampled.AttachTexture(GL_COLOR_ATTACHMENT0, textColMultAtt, 0);
  _fboMultisampled.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthStencMultAtt);

  if (_fboMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  /* Configure second post - processing framebuffer */
  _fboIntermediate.Create();

  /* Create normal color attachment texture */
  Texture2D textColAtt;
  textColAtt.format = GL_RGB;
  textColAtt.internalformat = GL_RGB8;
  textColAtt.Create(GL_TEXTURE_2D);
  textColAtt.CreateStorage(width, height);
  textColAtt.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  textColAtt.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  _fboIntermediate.AttachTexture(GL_COLOR_ATTACHMENT0, textColAtt, 0);

  if (_fboIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
}
void Engine::CreateFramebufferShadowMap(int width, int height)
{
  _fboShadowMap.Create();

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
  depthMap.SetParameterfv(GL_TEXTURE_BORDER_COLOR, array<float, 4>{ 1.0, 1.0, 1.0, 1.0 }.data());

  /* With the generated depth texture we can attach it as the framebuffer's depth buffer */
  _fboShadowMap.AttachTexture(GL_DEPTH_ATTACHMENT, depthMap, 0);
}
void Engine::CreateScreenSquare()
{
  _screenSquare.Create();

  float vertices[] = {
    /* positions   texCoords */
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  //Buffer vbo(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  Buffer vbo;
  vbo.target = GL_ARRAY_BUFFER;
  vbo.Create();
  vbo.CreateStorage(sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  _screenSquare.AttachVertexBuffer(0, vbo, 0, 4 * sizeof(float));

  VertexSpecifications specs{};
  specs.attrindex = 0;
  specs.bindingindex = 0;
  specs.components = 2;
  specs.normalized = true;
  specs.relativeoffset = 0;
  specs.type = GL_FLOAT;
  _screenSquare.SetVertexSpecifications(specs);

  specs.attrindex = 1;
  specs.bindingindex = 0;
  specs.components = 2;
  specs.normalized = true;
  specs.relativeoffset = 2 * sizeof(float);
  specs.type = GL_FLOAT;
  _screenSquare.SetVertexSpecifications(specs);

  
  _screenSquare.numVertices = 6;
  _screenSquare.numIndices = 0;
}
void Engine::ResizeFramebuffer(vec2i32 newViewportSize)
{
  _viewport = newViewportSize;
  _fboMultisampled.Delete();
  _fboIntermediate.Delete();
  CreateFramebuffer(4, newViewportSize.x, newViewportSize.y);
}
