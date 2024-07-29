#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/PrimaryCamera.hpp"
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
vec3f lightPosition = vec3f{ 0.0f, 2.0f, -7.0f };

static void RenderDirectionalLight(Program* program, const Components::DirectionalLight& light)
{
  program->SetUniform3f("u_directionalLight.color", light.color);
  program->SetUniform1f("u_directionalLight.diffuseIntensity", light.diffuseIntensity);
  program->SetUniform1f("u_directionalLight.specularIntensity", light.specularIntensity);
  program->SetUniform3f("u_directionalLight.direction", light.direction);
}
static void RenderPointLight(Program* program, const Components::PointLight& light, int i)
{
  program->SetUniform3f(std::format("u_pointLight[{}].color", i).c_str(), light.color);
  program->SetUniform1f(std::format("u_pointLight[{}].diffuseIntensity", i).c_str(), light.diffuseIntensity);
  program->SetUniform1f(std::format("u_pointLight[{}].specularIntensity", i).c_str(), light.specularIntensity);
  program->SetUniform3f(std::format("u_pointLight[{}].position", i).c_str(), light.position);
  program->SetUniform1f(std::format("u_pointLight[{}].attenuation.kl", i).c_str(), light.attenuation.kl);
  program->SetUniform1f(std::format("u_pointLight[{}].attenuation.kq", i).c_str(), light.attenuation.kq);
}
static void RenderSpotLight(Program* program, const Components::SpotLight& light)
{
  program->SetUniform3f("u_spotLight.color", light.color);
  program->SetUniform1f("u_spotLight.diffuseIntensity", light.diffuseIntensity);
  program->SetUniform1f("u_spotLight.specularIntensity", light.specularIntensity);
  program->SetUniform3f("u_spotLight.direction", light.direction);
  program->SetUniform3f("u_spotLight.position", light.position);
  program->SetUniform1f("u_spotLight.attenuation.kl", light.attenuation.kl);
  program->SetUniform1f("u_spotLight.attenuation.kq", light.attenuation.kq);
  program->SetUniform1f("u_spotLight.cutOff", light.cutOff);
  program->SetUniform1f("u_spotLight.outerCutOff", light.outerCutOff);
}
static void RenderScene(Scene& scene, Program* program)
{
  scene.Reg().view<Components::DirectionalLight>().each([&](auto& light) {
    RenderDirectionalLight(program, light);
  });

  int i = 0;
  scene.Reg().view<Components::PointLight>().each([&](auto& light) {
    RenderPointLight(program, light, i);
    i++;
  });

  scene.Reg().view<Components::SpotLight>().each([&](auto& light) {
    RenderSpotLight(program, light);
  });

  scene.Reg().view<Components::Model, Components::Transform>().each([&](auto& model, auto& transform) {
    program->SetUniformMat4f("u_model", transform.GetTransformation());
    model.DrawModel(DRAW_MODE);
  });
}
static void CreateSkybox(VertexArray& skybox, TextureCubemap& skyboxTexture)
{
  float skyboxVertices[] = {
    /* positions */
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

  skybox.Create();
  skybox.AttachVertexBuffer(0, vbo, 0, 3 * sizeof(float));
  skybox.EnableAttribute(0);
  skybox.SetAttribBinding(0, 0);
  skybox.SetAttribFormat(0, 3, GL_FLOAT, true, 0);
  skybox.numIndices = 0;
  skybox.numVertices = 36;

  const array<fspath, 6> faces = {
    fspath(TEXTURES_PATH / "skybox/right.jpg"),
    fspath(TEXTURES_PATH / "skybox/left.jpg"),
    fspath(TEXTURES_PATH / "skybox/top.jpg"),
    fspath(TEXTURES_PATH / "skybox/bottom.jpg"),
    fspath(TEXTURES_PATH / "skybox/front.jpg"),
    fspath(TEXTURES_PATH / "skybox/back.jpg"),
  };
  skyboxTexture.size = 2048;
  skyboxTexture.Create();
  skyboxTexture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  skyboxTexture.LoadImages(faces);
}

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  Logger::Initialize();
  CONSOLE_INFO("Logger ready");

  /* Initialize window */
  g_windowManager.Initialize();
  CONSOLE_INFO("WindowManager ready");
  
  g_windowManager.SetWindowTitle("ProjectGL");
  g_windowManager.SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  g_windowManager.SetWindowPosition(50, 50);
  g_windowManager.SetWindowAspectRatio(16, 9);
  g_windowManager.SetWindowVsync(false);

  /* initialize viewport */
  _viewportSize.x = WINDOW_WIDTH;
  _viewportSize.y = WINDOW_HEIGHT;

  /* Setup ImGui context */
  ImGuiLayer::SetupContext();
  ImGuiLayer::SetFont((FONTS_PATH / "Karla-Regular.ttf"), 16);
  CONSOLE_INFO("ImGuiLayer ready");
  
  /* Initialize shaders */
  g_shaderManager.LoadShadersFromDir(SHADERS_PATH);
  g_shaderManager.LoadPrograms();
  g_shaderManager.SetUpProgramsUniforms();
  CONSOLE_INFO("ShaderManager ready!");

  /* Initialize textures */
  g_textureManager.LoadTexturesFromDir(TEXTURES_PATH);
  CONSOLE_INFO("TextureManager ready!");

  /* Create Framebuffer object */
  CreateFramebuffer(4, _viewportSize.x, _viewportSize.y);
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
  TextureCubemap skyboxTexture;
  VertexArray skybox;
  CreateSkybox(skybox, skyboxTexture);

  /* -------------------------- Camera -------------------------- */
  PrimaryCamera camera(
    vec3f(0.0f, 2.0f, 5.0f),
    45.0f, 
    static_cast<float>(_viewportSize.x) / static_cast<float>(_viewportSize.y), 
    Z_NEAR, 
    Z_FAR
  );

  /* -------------------------- Scene -------------------------- */
  Scene scene;
  scene.LoadScene((ROOT_PATH / "Scene.ini"));

  /* -------------------------- Shadow map -------------------------- */
  mat4f lightProjection{};
  mat4f lightView{};
  mat4f lightSpaceMatrix{};
  vec3f* lightDirection = nullptr;
  scene.Reg().view<Components::DirectionalLight>().each([&lightDirection](auto& light) {
    lightDirection = &light.direction;
  });

  /* -------------------------- Pre-loop -------------------------- */
  Program* framebufferProgram = g_shaderManager.GetProgram("Framebuffer");
  Program* sceneProgram = g_shaderManager.GetProgram("Scene");
  Program* shadowMapDepthProgram = g_shaderManager.GetProgram("ShadowMapDepth");
  Program* shadowMapProgram = g_shaderManager.GetProgram("ShadowMap");
  Program* visualshadowDepthProgram = g_shaderManager.GetProgram("VisualShadowDepth");
  Program* skyboxProgram = g_shaderManager.GetProgram("Skybox");
  Texture2D& fboImageTexture = _fboIntermediate.GetTextureAttachment(0);
  Texture2D& fboImageTextureShadowMap = _fboShadowMap.GetTextureAttachment(0);

  time_point lastUpdateTime = system_clock::now();
  int toggleMode = 1;
  int useNormalMap = 1;
  int useParallaxMap = 0;
  
  /* -------------------------- loop -------------------------- */
  while (g_windowManager.IsOpen())
  {
    ImGuiLayer::BeginFrame();
    ImGuiLayer::Docking();

    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    const float time = glfwGetTime();
    drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    g_windowManager.PoolEvents();
    camera.ProcessInput(delta);

    if (g_windowManager.GetKey(GLFW_KEY_F1) == GLFW_PRESS) toggleMode = 1;
    else if (g_windowManager.GetKey(GLFW_KEY_F2) == GLFW_PRESS) toggleMode = 2;
    else if (g_windowManager.GetKey(GLFW_KEY_F3) == GLFW_PRESS) toggleMode = 3;

    if (g_windowManager.GetKey(GLFW_KEY_F5) == GLFW_PRESS) useNormalMap = 1;
    else if (g_windowManager.GetKey(GLFW_KEY_F6) == GLFW_PRESS) useNormalMap = 0;

    /* -------------------------- Update -------------------------- */
    const auto& cameraView = camera.cameraComponent->GetView();
    const auto& cameraProj = camera.cameraComponent->GetProjection();
    _uboCamera.UpdateStorage(0, sizeof(mat4f), &cameraView[0]);
    _uboCamera.UpdateStorage(sizeof(mat4f), sizeof(mat4f), &cameraProj[0]);
    lightProjection = Math::Ortho(LEFT, RIGHT, BOTTOM, TOP, Z_NEAR, Z_FAR);
    lightView = Math::LookAt(lightPosition, *lightDirection, vec3f(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;

    /* -------------------------- Rendering -------------------------- */
    /* Render depth of scene to texture(from directional light's perspective) */
    _fboShadowMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      shadowMapDepthProgram->Use();
      shadowMapDepthProgram->SetUniformMat4f("u_lightSpaceMatrix", lightSpaceMatrix);
      scene.Reg().view<Components::Model, Components::Transform>().each([&](auto& model, auto& transform) {
        shadowMapDepthProgram->SetUniformMat4f("u_model", transform.GetTransformation());
        std::for_each_n(model.meshes, model.numMeshes, [](auto& mesh) {
          mesh.DrawMesh(DRAW_MODE);
        });
      });
    }
    _fboShadowMap.Unbind(GL_FRAMEBUFFER);

    /* Fill the framebuffer color texture */
    _fboMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, _viewportSize.x, _viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      switch (toggleMode)
      {
      case 1: /* Render scene with no shadows */
        sceneProgram->Use();
        sceneProgram->SetUniform3f("u_viewPos", camera.cameraComponent->position);
        sceneProgram->SetUniform3f("u_ambientLightColor", AMBIENT_COLOR);
        sceneProgram->SetUniform1f("u_ambientLightIntensity", AMBIENT_INTENSITY);
        sceneProgram->SetUniform1i("u_useNormalMap", useNormalMap);
        RenderScene(scene, sceneProgram);
        break;

      case 2: /* Render scene with shadows map */
        shadowMapProgram->Use();
        shadowMapProgram->SetUniform3f("u_viewPos", camera.cameraComponent->position);
        shadowMapProgram->SetUniformMat4f("u_lightSpaceMatrix", lightSpaceMatrix);
        shadowMapProgram->SetUniform3f("u_ambientLightColor", AMBIENT_COLOR);
        shadowMapProgram->SetUniform1f("u_ambientLightIntensity", AMBIENT_INTENSITY);
        shadowMapProgram->SetUniform1i("u_useNormalMap", useNormalMap);
        fboImageTextureShadowMap.BindTextureUnit(10);
        RenderScene(scene, shadowMapProgram);
        break;

      case 3: /* Render Depth map texture for visual debugging */
        visualshadowDepthProgram->Use();
        fboImageTextureShadowMap.BindTextureUnit(0);
        Renderer::DrawArrays(DRAW_MODE, _screenSquare);
        break;
      }

      /* Draw skybox as last */
      {
        skyboxProgram->Use();
        skyboxProgram->SetUniformMat4f("u_projection", cameraProj);
        skyboxProgram->SetUniformMat4f("u_view", mat4f(mat3f(cameraView)));
        skyboxTexture.BindTextureUnit(0);
        Depth::SetFunction(GL_LEQUAL); 
        Renderer::DrawArrays(GL_TRIANGLES, skybox);
        Depth::SetFunction(GL_LESS); 
      }

      /* Blit multisampled buffer to normal colorbuffer of intermediate FBO */
      _fboMultisampled.Blit(_fboIntermediate,
        0, 0, _viewportSize.x, _viewportSize.y,
        0, 0, _viewportSize.x, _viewportSize.y,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
    }
    _fboMultisampled.Unbind(GL_FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* Render to GLFW framebuffer */
    //framebufferProgram->Use();
    //fboImageTexture.BindTextureUnit(0);
    //Renderer::DrawArrays(GL_TRIANGLES, _screenSquare);

    ImGuiLayer::RenderDemo();
    ImGuiLayer::RenderMenuBar(scene);
    ImGuiLayer::RenderGlobals();
    GameObject objectSelected = ImGuiLayer::RenderOutlinerPanel(scene);
    if (objectSelected.IsValid()) 
      ImGuiLayer::RenderDetails(objectSelected);
    vec2i32 viewport = ImGuiLayer::RenderViewportAndGuizmo(fboImageTexture, objectSelected, cameraView, cameraProj);
    ImGuiLayer::EndFrame();

    /* Checking viewport size and resize framebuffer */
    if (viewport != _viewportSize)
    {
      _viewportSize = viewport;
      
      ResizeFramebuffer(viewport.x, viewport.y);
      camera.cameraComponent->aspect = static_cast<float>(viewport.x) / static_cast<float>(viewport.y);
      camera.cameraComponent->UpdateProjection();
    }

    g_windowManager.SwapWindowBuffers();
    lastUpdateTime = now;
  }

  skybox.Delete();
  skyboxTexture.Delete();
}
void Engine::CleanUp()
{
  _fboIntermediate.Delete();
  _fboMultisampled.Delete();
  _screenSquare.Delete();
  _uboCamera.Delete();

  ImGuiLayer::CleanUp();

  g_shaderManager.CleanUp();
  g_textureManager.CleanUp();
  g_windowManager.CleanUp();
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Gamma correction OFF */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  glEnable(GL_MULTISAMPLE);

  glClearColor(0.15, 0.15, 0.15, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);
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
void Engine::ResizeFramebuffer(int width, int height)
{
  _viewportSize = vec2i32(width, height);
  _fboMultisampled.Delete();
  _fboIntermediate.Delete();
  CreateFramebuffer(4, width, height);
}
