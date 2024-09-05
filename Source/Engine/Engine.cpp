#include "Engine.hpp"

#include "Core/GL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Vertex.hpp"
#include "Engine/Graphics/Depth.hpp"
#include "Engine/Graphics/Stencil.hpp"
#include "Engine/Graphics/Culling.hpp"
#include "Engine/Graphics/Objects/RenderBuffer.hpp"
#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Graphics/Objects/TextureCubemap.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/Globals.hpp"

#include "GUI/ImGuiLayer.hpp"

#include <GLFW/glfw3.h>

static void GLAPIENTRY MessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam)
{
  /* Ignore non-significant error/warning codes */ 
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
    return;

  CONSOLE_ERROR("GL callback: {} ", message);
}

static void SetOpenGLStates()
{
  /* Enable debug output */ 
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

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

  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);
}

static void RenderDirectionalLight(Program* program, const Components::DirectionalLight& light)
{
  program->SetUniform3f("u_directionalLight.color", light.color);
  program->SetUniform1f("u_directionalLight.diffuseIntensity", light.diffuseIntensity);
  program->SetUniform1f("u_directionalLight.specularIntensity", light.specularIntensity);
  program->SetUniform3f("u_directionalLight.direction", light.direction);
}
static void RenderPointLight(Program* program, const Components::PointLight& light, i32 i)
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

  i32 i = 0;
  scene.Reg().view<Components::PointLight>().each([&](auto& light) {
    RenderPointLight(program, light, i);
    i++;
  });

  scene.Reg().view<Components::SpotLight>().each([&](auto& light) {
    RenderSpotLight(program, light);
  });

  scene.Reg().view<Components::Model, Components::Transform>().each([&](auto& model, auto& transform) {
    program->SetUniformMat4f("u_model", transform.GetTransformation());
    model.DrawModel(GL_TRIANGLES);
  });
}
static void CreateSkybox(VertexArray& skybox, TextureCubemap& skyboxTexture)
{
  f32 vertices[] = {
    /* Position */
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
  Buffer vbo(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  skybox.Create();
  skybox.AttachVertexBuffer(0, vbo.id, 0, sizeof(Vertex_P));
  skybox.EnableAttribute(0);
  skybox.SetAttribBinding(0, 0);
  skybox.SetAttribFormat(0, 3, GL_FLOAT, true, 0);
  skybox.numIndices = 0;
  skybox.numVertices = 36;

  TextureManager& textureManager = TextureManager::Get();
  const Array<Texture2D*, 6> images = {
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/right.jpg"),
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/left.jpg"),
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/top.jpg"),
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/bottom.jpg"),
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/front.jpg"),
    textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/back.jpg"),
  };
  i32 cubemapInternalFormat = images.at(0)->internalFormat;
  i32 width = images.at(0)->width;
  i32 height = images.at(0)->height;

  skyboxTexture.Create();
  skyboxTexture.CreateStorage(cubemapInternalFormat, width, height);
  skyboxTexture.LoadImages(images);

  skyboxTexture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

static FrameBuffer CreateDepthMapFbo(i32 width, i32 height)
{
  FrameBuffer fbo;
  fbo.Create();

  /* Create a 2D texture that we'll use as the framebuffer's depth buffer */
  Texture2D depthMap(GL_TEXTURE_2D);
  depthMap.Create();
  depthMap.CreateStorage(GL_DEPTH_COMPONENT24, width, height);
  depthMap.SetParameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  depthMap.SetParameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  depthMap.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  depthMap.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* Resolve the problem of over sampling */
  depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  depthMap.SetParameterfv(GL_TEXTURE_BORDER_COLOR, Array<f32, 4>{ 1.0, 1.0, 1.0, 1.0 }.data());

  /* With the generated depth texture we can attach it as the framebuffer's depth buffer */
  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, depthMap.id, 0);
  return fbo;
}
static FrameBuffer CreateDepthCubeMapFbo(i32 width, i32 height)
{
  FrameBuffer fbo;
  fbo.Create();

  TextureCubemap texture;
  texture.Create();
  texture.CreateStorage(GL_DEPTH_COMPONENT24, width, height);
  for (i32 i = 0; i < 6; i++)
    texture.SubImage3D(0, 0, 0, i, width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, texture.id, 0);
  return fbo;
}

static Texture2D CreateDefaultTexture(Array<u8, 3> textureData)
{
  Texture2D texture(GL_TEXTURE_2D);
  texture.Create();
  texture.CreateStorage(GL_RGB8, 1, 1);
  texture.UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, textureData.data());
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return texture;
}
static VertexArray CreateTerrain(i32 rez)
{
  VertexArray vao;
  vao.Create();
  vao.SetAttribFormat(0, 3, GL_FLOAT, false, offsetof(Vertex_P_UV, position));
  vao.SetAttribBinding(0, 0);
  vao.EnableAttribute(0);

  vao.SetAttribFormat(1, 2, GL_FLOAT, false, offsetof(Vertex_P_UV, uv));
  vao.SetAttribBinding(1, 0);
  vao.EnableAttribute(1);

  Texture2D* heightMap = TextureManager::Get().GetTextureByPath(TEXTURES_PATH / "iceland_heightmap.png");
  i32 width = heightMap->width;
  i32 height = heightMap->height;

  // vertex generation
  Vector<f32> vertices;
  for (i32 i = 0; i <= rez - 1; i++)
  {
    for (i32 j = 0; j <= rez - 1; j++)
    {
      vertices.push_back(-width / 2.0f + width * i / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * j / (f32)rez); // v.z
      vertices.push_back(i / (f32)rez); // u
      vertices.push_back(j / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * j / (f32)rez); // v.z
      vertices.push_back((i + 1) / (f32)rez); // u
      vertices.push_back(j / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * i / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / (f32)rez); // v.z
      vertices.push_back(i / (f32)rez); // u
      vertices.push_back((j + 1) / (f32)rez); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / (f32)rez); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / (f32)rez); // v.z
      vertices.push_back((i + 1) / (f32)rez); // u
      vertices.push_back((j + 1) / (f32)rez); // v
    }
  }
  Buffer vbo(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);
  vao.AttachVertexBuffer(0, vbo.id, 0, sizeof(Vertex_P_UV));

  vao.numVertices = 4 * pow(rez, 2);
  vao.numIndices = 0;

  return vao;
}

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  /* Initialize logger */
  /* ----------------- */
  Logger::Initialize();
  CONSOLE_INFO("Logger ready");

  /* Initialize window manager */
  /* ------------------------- */
  WindowManager& windowManager = WindowManager::Get();
  windowManager.Initialize(WindowProps(
    vec2i32{ WINDOW_WIDTH, WINDOW_HEIGHT }, 
    vec2i32{ 50, 50 }, 
    "GameEngine", 
    vec2i32{ 16, 9 },
    false
  ));
  CONSOLE_INFO("WindowManager ready");

  /* Setup ImGui context */
  /* ------------------- */
  ImGuiLayer::SetupContext();
  ImGuiLayer::SetFont((FONTS_PATH / "Karla-Regular.ttf"), 16);
  CONSOLE_INFO("ImGuiLayer ready");
  
  /* Initialize shader manager */
  /* ------------------------- */
  ShaderManager& shaderManager = ShaderManager::Get();
  shaderManager.LoadShadersFromDir(SHADERS_PATH);
  shaderManager.LoadPrograms();
  shaderManager.SetUpProgramsUniforms();
  CONSOLE_INFO("ShaderManager ready!");

  /* Initialize texture manager */
  /* -------------------------- */
  /* !!The first 4 positions are reserved for the default textures */
  Texture2D defaultDiffuseTexture = CreateDefaultTexture(Array<u8, 3>{ 128, 128, 255 });
  Texture2D defaultSpecularTexture = CreateDefaultTexture(Array<u8, 3>{ 255, 255, 255 });
  Texture2D defaultNormalTexture = CreateDefaultTexture(Array<u8, 3>{ 0, 0, 0 });
  Texture2D defaultHeightTexture = CreateDefaultTexture(Array<u8, 3>{ 0, 0, 0 });
  
  TextureManager& textureManager = TextureManager::Get();
  textureManager.LoadTexture(defaultDiffuseTexture);
  textureManager.LoadTexture(defaultSpecularTexture);
  textureManager.LoadTexture(defaultNormalTexture);
  textureManager.LoadTexture(defaultHeightTexture);
  textureManager.LoadTexturesFromDir(TEXTURES_PATH);
  CONSOLE_INFO("TextureManager ready!");

  /* Create Framebuffer object */
  /* ------------------------- */
  CreateFramebuffer(4, WINDOW_WIDTH, WINDOW_HEIGHT);
  CreateScreenSquare();

  /* Initialize uniform block objects */
  /* -------------------------------- */
  /* Reserve memory for both projection,view matrices */
  _uboCamera = Buffer(GL_UNIFORM_BUFFER, 2 * sizeof(mat4f), nullptr, GL_STATIC_DRAW);
  _uboCamera.BindBase(0); /* cameraBlock to bindingpoint 0 */

  /* Set viewport */
  /* ------------ */
  _viewportSize.x = WINDOW_WIDTH;
  _viewportSize.y = WINDOW_HEIGHT;

  SetOpenGLStates();
}
void Engine::Run()
{
  /* Create skybox object */
  TextureCubemap skyboxTexture;
  VertexArray skybox;
  CreateSkybox(skybox, skyboxTexture);

  /* Create scene object */
  Scene scene(ROOT_PATH / "Scene.ini");

  Components::DirectionalLight* directionalLight = nullptr;
  scene.Reg().view<Components::DirectionalLight>().each([&](auto& light) { directionalLight = &light; });

  Components::PointLight* pointLight = nullptr;
  scene.Reg().view<Components::PointLight>().each([&](auto& light) { pointLight = &light; });

  /* Create primary camera object */
  Camera primaryCamera(vec3f(20.0f, 6.0f, 0.0f));
  primaryCamera.yaw = 180.0f;
  primaryCamera.frustum.zFar = 100.0f;

  /* Setting up the directional shadow mapping  */
  FrameBuffer fboDepthMap = CreateDepthMapFbo(1024, 1024);

  Camera directLightCamera(vec3f(0.0f, 10.0f, 10.0f));
  directLightCamera.frustum.zFar = 30.0f;

  /* Setting up the omnidirectional shadow mapping */
  FrameBuffer fboDepthCubeMap = CreateDepthCubeMapFbo(1024, 1024);

  /* Prepare vertices for terrain */
  i32 rez = 20;
  VertexArray terrain = CreateTerrain(rez);

  /* ---------------------------------------------------------------------- */
  /* -------------------------- Pre-loop section -------------------------- */
  /* ---------------------------------------------------------------------- */
  WindowManager& windowManager = WindowManager::Get();
  ShaderManager& shaderManager = ShaderManager::Get();
  TextureManager& textureManager = TextureManager::Get();

  Program* framebufferProgram = shaderManager.GetProgram("Framebuffer");
  Program* sceneProgram = shaderManager.GetProgram("Scene");
  Program* sceneShadowsProgram = shaderManager.GetProgram("SceneShadows");
  Program* depthMapProgram = shaderManager.GetProgram("DepthMap");
  Program* depthCubeMapProgram = shaderManager.GetProgram("DepthCubeMap");
  Program* skyboxProgram = shaderManager.GetProgram("Skybox");
  Program* terrainProgram = shaderManager.GetProgram("Terrain");
  u32 fboTexture = _fboIntermediate.GetTextureAttachment(0);
  u32 depthMapTexture = fboDepthMap.GetTextureAttachment(0);
  u32 depthCubeMapTexture = fboDepthCubeMap.GetTextureAttachment(0);
  Texture2D* heightMap = textureManager.GetTextureByPath(TEXTURES_PATH / "iceland_heightmap.png");

  chrono::steady_clock::time_point now{};
  chrono::steady_clock::time_point lastFrameTime{};
  f64 delta = 0.0f; /* Time elapsed between two frames */
  
  chrono::steady_clock::time_point timerT0 = chrono::steady_clock::now();
  chrono::steady_clock::time_point timerT1{};
  i32 frames = 0; 
  i32 frameRate = 0; /* How many frames generated per seconds */
  f64 totalDeltasPerSecond = 0.0f; 
  f64 avgTime = 0.0f; /* The average rendering time per seconds */

  bool renderingShadowsMode = true;
  bool useNormalMap = true;
  bool wireframeEnabled = false;

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  
  /* ------------------------------------------------------------------ */
  /* -------------------------- loop section -------------------------- */
  /* ------------------------------------------------------------------ */
  while (windowManager.IsOpen())
  {
    ImGuiLayer::BeginFrame();
    ImGuiLayer::Docking();

    /* ---------------------------------------------------------------------------------- */
    /* -------------------------- Per-frame time logic section -------------------------- */
    /* ---------------------------------------------------------------------------------- */
    now = chrono::steady_clock::now();
    delta = chrono::duration_cast<chrono::duration<f64>>(now - lastFrameTime).count();
    lastFrameTime = now;
    totalDeltasPerSecond += delta;
    timerT1 = chrono::steady_clock::now();
    f64 timer_diff = chrono::duration_cast<chrono::duration<f64>>(timerT1 - timerT0).count();
    if (timer_diff >= 1)
    {
      timerT0 = chrono::steady_clock::now();
      avgTime = totalDeltasPerSecond / frames;
      frameRate = frames;
      frames = 0;
      totalDeltasPerSecond = 0;
    }

    g_drawCalls = 0;

    /* ------------------------------------------------------------------- */
    /* -------------------------- Input section -------------------------- */
    /* ------------------------------------------------------------------- */
    windowManager.PoolEvents();
    primaryCamera.ProcessKeyboard(delta, 5.0f);
    primaryCamera.ProcessMouse(delta, 15.0f);

    if (windowManager.GetKey(GLFW_KEY_F1) == GLFW_PRESS) renderingShadowsMode = false;
    else if (windowManager.GetKey(GLFW_KEY_F2) == GLFW_PRESS) renderingShadowsMode = true;
    if (windowManager.GetKey(GLFW_KEY_F5) == GLFW_PRESS) useNormalMap = false;
    else if (windowManager.GetKey(GLFW_KEY_F6) == GLFW_PRESS) useNormalMap = true;
    if (windowManager.GetKey(GLFW_KEY_F9) == GLFW_PRESS) wireframeEnabled = false;
    else if (windowManager.GetKey(GLFW_KEY_F10) == GLFW_PRESS) wireframeEnabled = true;

    /* -------------------------------------------------------------------- */
    /* -------------------------- Update section -------------------------- */
    /* -------------------------------------------------------------------- */
    primaryCamera.UpdateOrientation();
    mat4f cameraView = primaryCamera.CalculateView(primaryCamera.position + primaryCamera.GetFrontVector());
    mat4f cameraProj = primaryCamera.CalculatePerspective(static_cast<f32>(_viewportSize.x) / static_cast<f32>(_viewportSize.y));
    _uboCamera.UpdateStorage(0, sizeof(mat4f), &cameraView[0]);
    _uboCamera.UpdateStorage(sizeof(mat4f), sizeof(mat4f), &cameraProj[0]);

    directLightCamera.UpdateOrientation();
    mat4f directLightProjection = directLightCamera.CalculateOrtho();
    mat4f directLightView = directLightCamera.CalculateView(directionalLight->direction);

    /* ----------------------------------------------------------------------- */
    /* -------------------------- Rendering section -------------------------- */
    /* ----------------------------------------------------------------------- */

    /* Fill the depth map from directional light's perspective */
    fboDepthMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      depthMapProgram->Use();
      depthMapProgram->SetUniformMat4f("u_lightView", directLightView);
      depthMapProgram->SetUniformMat4f("u_lightProjection", directLightProjection);
      scene.Reg().view<Components::Model, Components::Transform>().each([&](auto& model, auto& transform) {
        depthMapProgram->SetUniformMat4f("u_model", transform.GetTransformation());
        std::for_each(model.meshes.begin(), model.meshes.end(), [&](auto& mesh) {
          mesh.DrawMesh(GL_TRIANGLES);
        });
      });
    }
    fboDepthMap.Unbind(GL_FRAMEBUFFER);

    /* Fill the depth map from point light's perspective */
    fboDepthCubeMap.Bind(GL_FRAMEBUFFER);
    {
      glViewport(0, 0, 1024, 1024);
      glClear(GL_DEPTH_BUFFER_BIT);

      const vec3f& lightPos = pointLight->position;
      mat4f pointLightProj = Math::Perspective(Math::Radians(90.0f), 1.0f, 0.1f, 15.0f);

      Array<mat4f, 6> pointLightViews{};
      pointLightViews[0] = Math::LookAt(lightPos, lightPos + vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f));
      pointLightViews[1] = Math::LookAt(lightPos, lightPos + vec3f(-1.0f, 0.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f));
      pointLightViews[2] = Math::LookAt(lightPos, lightPos + vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f));
      pointLightViews[3] = Math::LookAt(lightPos, lightPos + vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 0.0f, -1.0f));
      pointLightViews[4] = Math::LookAt(lightPos, lightPos + vec3f(0.0f, 0.0f, 1.0f), vec3f(0.0f, -1.0f, 0.0f));
      pointLightViews[5] = Math::LookAt(lightPos, lightPos + vec3f(0.0f, 0.0f, -1.0f), vec3f(0.0f, -1.0f, 0.0f));

      depthCubeMapProgram->Use();
      depthCubeMapProgram->SetUniformMat4f("u_lightProjection", pointLightProj);
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[0]", pointLightViews.at(0));
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[1]", pointLightViews.at(1));
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[2]", pointLightViews.at(2));
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[3]", pointLightViews.at(3));
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[4]", pointLightViews.at(4));
      depthCubeMapProgram->SetUniformMat4f("u_lightViews[5]", pointLightViews.at(5));
      depthCubeMapProgram->SetUniform3f("u_lightPos", lightPos);
      depthCubeMapProgram->SetUniform1f("u_zFar", 15.0f);

      scene.Reg().view<Components::Model, Components::Transform>().each([&](auto& model, auto& transform) {
        depthCubeMapProgram->SetUniformMat4f("u_model", transform.GetTransformation());
        std::for_each(model.meshes.begin(), model.meshes.end(), [&](auto& mesh) {
          mesh.DrawMesh(GL_TRIANGLES);
          });
        });
    }
    fboDepthCubeMap.Unbind(GL_FRAMEBUFFER);

    /* Fill the framebuffer color texture */
    _fboMultisampled.Bind(GL_FRAMEBUFFER);
    { 
      glViewport(0, 0, _viewportSize.x, _viewportSize.y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      glPolygonMode(GL_FRONT, wireframeEnabled ? GL_LINE : GL_FILL);
      glPolygonMode(GL_BACK, wireframeEnabled ? GL_LINE : GL_FILL);

      ///* Render terrain */
      //{
      //  terrainProgram->Use();
      //  terrainProgram->SetUniformMat4f("u_model", mat4f(1.0f));
      //  terrainProgram->SetUniformMat4f("u_view", cameraView);
      //  terrainProgram->SetUniformMat4f("u_projection", cameraProj);
      //  terrainProgram->SetUniform1i("u_heightMap", 0);
      //  heightMap->BindTextureUnit(0);
      //  Renderer::DrawArrays(GL_PATCHES, terrain);
      //}

      /* Render scene with shadows map */
      if (renderingShadowsMode)
      {
        sceneShadowsProgram->Use();
        sceneShadowsProgram->SetUniform3f("u_viewPos", primaryCamera.position);
        sceneShadowsProgram->SetUniform3f("u_ambientLightColor", g_ambientColor);
        sceneShadowsProgram->SetUniform1f("u_ambientLightIntensity", g_ambientIntensity);
        sceneShadowsProgram->SetUniformMat4f("u_lightView", directLightView);
        sceneShadowsProgram->SetUniformMat4f("u_lightProjection", directLightProjection);
        sceneShadowsProgram->SetUniform1i("u_useNormalMap", useNormalMap);
        glBindTextureUnit(10, depthMapTexture);
        glBindTextureUnit(11, depthCubeMapTexture);
        RenderScene(scene, sceneShadowsProgram);
      }
      /* Render scene with no shadows */
      else
      {
        sceneProgram->Use();
        sceneProgram->SetUniform3f("u_viewPos", primaryCamera.position);
        sceneProgram->SetUniform3f("u_ambientLightColor", g_ambientColor);
        sceneProgram->SetUniform1f("u_ambientLightIntensity", g_ambientIntensity);
        sceneProgram->SetUniform1i("u_useNormalMap", useNormalMap);
        RenderScene(scene, sceneProgram);
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

    //ImGuiLayer::Demo();
    ImGuiLayer::MenuBar(scene);
    ImGuiLayer::ApplicationInfo(delta, avgTime, frameRate);
    ImGuiLayer::CameraProps("Primary camera", primaryCamera);
    ImGuiLayer::CameraProps("DirecLight camera", directLightCamera);
    //ImGuiLayer::RenderWorld();
    ImGuiLayer::DebugDepthMap(depthMapTexture);

    GameObject objectSelected = ImGuiLayer::OutlinerPanel(scene);
    ImGuiLayer::GameObjectDetails(objectSelected);
    vec2i32 viewport = ImGuiLayer::ViewportGizmo(fboTexture, objectSelected, cameraView, cameraProj);

    ImGuiLayer::EndFrame();

    /* Checking viewport size */
    if (viewport != _viewportSize)
    {
      _viewportSize = viewport;
      _fboMultisampled.Delete();
      _fboIntermediate.Delete();
      CreateFramebuffer(4, viewport.x, viewport.y);
    }

    /* ------------------------------------------------------------------ */
    /* -------------------------- Swap buffers -------------------------- */
    /* ------------------------------------------------------------------ */
    windowManager.SwapWindowBuffers();

    frames++;
  }

  fboDepthCubeMap.Delete();
  fboDepthMap.Delete();

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

  ShaderManager::Get().CleanUp();
  TextureManager::Get().CleanUp();
  WindowManager::Get().CleanUp(); /* !!Raise exception here */
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Engine::CreateFramebuffer(i32 samples, i32 width, i32 height)
{
  _fboMultisampled.Create();

  /* Create a multisampled color attachment texture */
  Texture2D textColMultAtt(GL_TEXTURE_2D_MULTISAMPLE);
  textColMultAtt.Create();
  textColMultAtt.CreateStorageMultisampled(GL_RGB8, samples, width, height);

  /* Create a multisampled renderbuffer object for depth and stencil attachments */
  RenderBuffer depthStencMultAtt;
  depthStencMultAtt.Create();
  depthStencMultAtt.CreateStorageMulstisampled(GL_DEPTH24_STENCIL8, samples, width, height);

  _fboMultisampled.AttachTexture(GL_COLOR_ATTACHMENT0, textColMultAtt.id, 0);
  _fboMultisampled.AttachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthStencMultAtt.id);

  if (_fboMultisampled.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Multisampled framebuffer is not complete!");

  /* Configure second post - processing framebuffer */
  _fboIntermediate.Create();

  /* Create normal color attachment texture */
  Texture2D textColAtt(GL_TEXTURE_2D);
  textColAtt.Create();
  textColAtt.CreateStorage(GL_RGB8, width, height);
  textColAtt.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  textColAtt.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  _fboIntermediate.AttachTexture(GL_COLOR_ATTACHMENT0, textColAtt.id, 0);

  if (_fboIntermediate.CheckStatus() != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_WARN("Intermediate framebuffer is not complete!");
}
void Engine::CreateScreenSquare()
{
  _screenSquare.Create();

  f32 vertices[] = {
    /* position    uv */
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  Buffer vbo(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  _screenSquare.AttachVertexBuffer(0, vbo.id, 0, 4 * sizeof(f32));

  _screenSquare.SetAttribFormat(0, 2, GL_FLOAT, true, 0);
  _screenSquare.SetAttribBinding(0, 0);
  _screenSquare.EnableAttribute(0);

  _screenSquare.SetAttribFormat(1, 2, GL_FLOAT, true, 2 * sizeof(f32));
  _screenSquare.SetAttribBinding(1, 0);
  _screenSquare.EnableAttribute(1);
  
  _screenSquare.numVertices = 6;
  _screenSquare.numIndices = 0;
}
