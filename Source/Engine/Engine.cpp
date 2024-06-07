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

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

int	INITIAL_WINDOW_W = 1600;
int	INITIAL_WINDOW_H = 900;
float GAMMA_CORRECTION = 2.2f;

float	Z_NEAR = 1.0f;
float	Z_FAR = 100.0f;
float LEFT = -30.0f;
float RIGHT = 30.0f;
float BOTTOM = -30.0f;
float TOP = 30.0f;

uint32_t drawCalls = 0;

static void RenderScene(Scene& scene, Program* sceneProgram)
{
  scene.Reg().view<DirLightComponent>().each([sceneProgram](auto& light) {
    sceneProgram->SetUniform3f("u_directionalLight.color", light.color);
    sceneProgram->SetUniform1f("u_directionalLight.ambient", light.ambient);
    sceneProgram->SetUniform1f("u_directionalLight.diffuse", light.diffuse);
    sceneProgram->SetUniform1f("u_directionalLight.specular", light.specular);
    sceneProgram->SetUniform3f("u_directionalLight.direction", light.direction);
  });

  int i = 0;
  scene.Reg().view<PointLightComponent>().each([sceneProgram, &i](auto& light) {
    sceneProgram->SetUniform3f(std::format("u_pointLight[{}].color", i).c_str(), light.color);
    sceneProgram->SetUniform1f(std::format("u_pointLight[{}].ambient", i).c_str(), light.ambient);
    sceneProgram->SetUniform1f(std::format("u_pointLight[{}].diffuse", i).c_str(), light.diffuse);
    sceneProgram->SetUniform1f(std::format("u_pointLight[{}].specular", i).c_str(), light.specular);
    sceneProgram->SetUniform3f(std::format("u_pointLight[{}].position", i).c_str(), light.position);
    sceneProgram->SetUniform1f(std::format("u_pointLight[{}].linear", i).c_str(), light.linear);
    sceneProgram->SetUniform1f(std::format("u_pointLight[{}].quadratic", i).c_str(), light.quadratic);
    i++;
  });

  scene.Reg().view<SpotLightComponent>().each([sceneProgram](auto& light) {
    sceneProgram->SetUniform3f("u_spotLight.color", light.color);
    sceneProgram->SetUniform1f("u_spotLight.ambient", light.ambient);
    sceneProgram->SetUniform1f("u_spotLight.diffuse", light.diffuse);
    sceneProgram->SetUniform1f("u_spotLight.specular", light.specular);
    sceneProgram->SetUniform3f("u_spotLight.direction", light.direction);
    sceneProgram->SetUniform3f("u_spotLight.position", light.position);
    sceneProgram->SetUniform1f("u_spotLight.linear", light.linear);
    sceneProgram->SetUniform1f("u_spotLight.quadratic", light.quadratic);
    sceneProgram->SetUniform1f("u_spotLight.cutOff", light.cutOff);
    sceneProgram->SetUniform1f("u_spotLight.outerCutOff", light.outerCutOff);
  });

  scene.Reg().view<StaticMeshComponent, TransformComponent>().each([sceneProgram](auto& mesh, auto& transform) {
    sceneProgram->SetUniformMat4f("u_model", transform.GetTransformation());
    mesh.Draw();
  });
}
static void LoadSkybox(VertexArray& vao)
{
  float skyboxVertices[] = {
    // positions 
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

  Buffer vbo(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  vao.Create();
  vao.AttachVertexBuffer(0, vbo, 0, 3 * sizeof(float));
  vao.numIndices = 0;
  vao.numVertices = 36;
  
  VertexSpecifications specs;
  specs.attrindex = 0;
  specs.bindingindex = 0;
  specs.components = 3;
  specs.normalized = true;
  specs.relativeoffset = 0;
  specs.type = GL_FLOAT;

  vao.SetVertexSpecifications(specs);
}

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
  _instanceWM->SetWindowSize(INITIAL_WINDOW_W, INITIAL_WINDOW_H);
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
  const array<string, 6> faces = {
    fspath(TEXTURES_PATH / "skybox/right.jpg").string(),
    fspath(TEXTURES_PATH / "skybox/left.jpg").string(),
    fspath(TEXTURES_PATH / "skybox/top.jpg").string(),
    fspath(TEXTURES_PATH / "skybox/bottom.jpg").string(),
    fspath(TEXTURES_PATH / "skybox/front.jpg").string(),
    fspath(TEXTURES_PATH / "skybox/back.jpg").string(),
  };
  
  TextureCubemap skyboxTexture;
  skyboxTexture.Create();
  skyboxTexture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  skyboxTexture.LoadImages(faces);
  
  VertexArray skyboxVAO;
  LoadSkybox(skyboxVAO);


  /* -------------------------- Camera -------------------------- */
  Camera camera(
    vec3f( 30.0f, 15.0f, 10.0f ),
    45.0f, 
    static_cast<float>(_viewport.x) / static_cast<float>(_viewport.y), 
    Z_NEAR, 
    Z_FAR
  );
  camera.cameraComponent->yaw = -180.0f;
  camera.cameraComponent->pitch = -30.0f;
  camera.cameraComponent->roll = 0.0f;
  camera.cameraComponent->UpdateVectors();

  /* -------------------------- Scene -------------------------- */
  Scene scene;
  scene.LoadScene((ROOT_PATH / "Scene.ini"));

  DirLightComponent* dirlight = nullptr;
  scene.Reg().view<DirLightComponent>().each([&dirlight](auto& light) {
    dirlight = &light;
  });

  /* -------------------------- Scene -------------------------- */
  const mat4f lightProjection = Math::Ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
  const vec3f lightPosition{ -2.0f, 20.0f, -1.0f };
  mat4f lightView{};
  mat4f lightSpaceMatrix{};


  /* -------------------------- Pre-loop -------------------------- */
  Program* framebufferProgram = _instanceSM->GetProgram("Framebuffer");
  Program* sceneProgram = _instanceSM->GetProgram("Scene");
  Program* shadowMapDepthProgram = _instanceSM->GetProgram("ShadowMapDepth");
  Program* shadowMapProgram = _instanceSM->GetProgram("ShadowMap");
  Program* visualshadowDepthProgram = _instanceSM->GetProgram("VisualShadowDepth");
  Program* skyboxProgram = _instanceSM->GetProgram("Skybox");

  Texture2D& fboImageTexture = _fboIntermediate.GetTextureAttachment(0);
  Texture2D& fboImageTextureShadowMap = _fboShadowMap.GetTextureAttachment(0);

  time_point lastUpdateTime = system_clock::now();

  int toggle = 2;
  
  /* -------------------------- loop -------------------------- */
  while (_instanceWM->IsOpen())
  {
    ImGuiLayer::NewFrame();
    ImGuiLayer::Docking();

    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    const float time = glfwGetTime();
    drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    _instanceWM->PoolEvents();
    camera.ProcessInput(delta);

    if (_instanceWM->GetKey(GLFW_KEY_1) == GLFW_PRESS)      toggle = 1; 
    else if (_instanceWM->GetKey(GLFW_KEY_2) == GLFW_PRESS) toggle = 2; 
    else if (_instanceWM->GetKey(GLFW_KEY_3) == GLFW_PRESS) toggle = 3; 

    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    _uboCamera.UpdateStorage(0, sizeof(mat4f), &cameraViewMatrix[0]);
    _uboCamera.UpdateStorage(sizeof(mat4f), sizeof(mat4f), &cameraProjectionMatrix[0]);

    lightView = Math::LookAt(lightPosition, vec3f(0), vec3f(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;

    
    /* -------------------------- Rendering -------------------------- */
    /* Render depth of scene to texture(from directional light's perspective) */
    _fboShadowMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      shadowMapDepthProgram->Use();
      shadowMapDepthProgram->SetUniformMat4f("u_lightSpaceMatrix", lightSpaceMatrix);
      scene.Reg().view<StaticMeshComponent, TransformComponent>().each([shadowMapDepthProgram](auto& mesh, auto& transform) {
        shadowMapDepthProgram->SetUniformMat4f("u_model", transform.GetTransformation());
        mesh.Draw();
      });
    }
    _fboShadowMap.Unbind(GL_FRAMEBUFFER);

    /* Fill the framebuffer color texture */
    _fboMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, _viewport.x, _viewport.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      /* Render scene with no shadows */
      if(toggle == 1)
      {
        sceneProgram->Use();
        sceneProgram->SetUniform3f("u_viewPos", camera.cameraComponent->position);
        RenderScene(scene, sceneProgram);
      }
      /* Render scene with shadows map */
      else if(toggle == 2)
      {
        shadowMapProgram->Use();
        shadowMapProgram->SetUniformMat4f("u_lightSpaceMatrix", lightSpaceMatrix);
        shadowMapProgram->SetUniform3f("u_viewPos", camera.cameraComponent->position);
        shadowMapProgram->SetUniform3f("u_lightPos", lightPosition);
        fboImageTextureShadowMap.BindTextureUnit(10);
        RenderScene(scene, shadowMapProgram);
      }
      /* Render Depth map texture for visual debugging */
      else if(toggle == 3)
      {
        visualshadowDepthProgram->Use();
        fboImageTextureShadowMap.BindTextureUnit(0);
        DrawArrays(GL_TRIANGLES, _screenSquare);
      }

      /* Draw skybox as last */
      {
        skyboxProgram->Use();
        skyboxProgram->SetUniformMat4f("u_projection", cameraProjectionMatrix);
        skyboxProgram->SetUniformMat4f("u_view", mat4f(mat3f(cameraViewMatrix)));
        skyboxTexture.BindTextureUnit(0);
        Depth::SetFunction(GL_LEQUAL); /* change depth function so depth test passes when values are equal to depth buffer's content */
        DrawArrays(GL_TRIANGLES, skyboxVAO);
        Depth::SetFunction(GL_LESS); /* set depth function back to default */
      }

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
    DrawArrays(GL_TRIANGLES, _screenSquare);

    //ImGuiLayer::RenderDemo();
    ImGuiLayer::RenderMenuBar(scene);
    auto objectSelected = ImGuiLayer::RenderOutlinerPanel(scene);
    if (objectSelected)
      ImGuiLayer::RenderDetails(objectSelected);
    ImGuiLayer::DrawData();
    
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
  /* Depth testing */
  Depth::EnableTest();
  Depth::EnableWritingBuffer();
  Depth::SetFunction(GL_LESS);

  /* Stencil testing */
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
