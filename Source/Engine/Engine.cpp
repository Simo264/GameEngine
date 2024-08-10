#include "Engine.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Camera.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Depth.hpp"
#include "Engine/Graphics/Stencil.hpp"
#include "Engine/Graphics/Culling.hpp"
#include "Engine/Graphics/RenderBuffer.hpp"
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

  glClearColor(0.15, 0.15, 0.15, 1.0f);
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
    model.DrawModel(g_drawMode);
  });
}
static void CreateSkybox(VertexArray& skybox, TextureCubemap& skyboxTexture)
{
  float vertices[] = {
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
  Buffer vbo(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  skybox.Create();
  skybox.AttachVertexBuffer(0, vbo.id, 0, 3 * sizeof(float));
  skybox.EnableAttribute(0);
  skybox.SetAttribBinding(0, 0);
  skybox.SetAttribFormat(0, 3, GL_FLOAT, true, 0);
  skybox.numIndices = 0;
  skybox.numVertices = 36;

  const array<Texture2D*, 6> images = {
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/right.jpg"),
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/left.jpg"),
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/top.jpg"),
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/bottom.jpg"),
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/front.jpg"),
    g_textureManager.GetTextureByPath(TEXTURES_PATH / "skybox/back.jpg"),
  };
  int cubemapInternalFormat = images.at(0)->internalFormat;
  int width = images.at(0)->width;
  int height = images.at(0)->height;

  skyboxTexture.Create();
  skyboxTexture.CreateStorage(cubemapInternalFormat, width, height);
  skyboxTexture.LoadImages(images);

  skyboxTexture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  skyboxTexture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

static void CreateDepthMapFbo(FrameBuffer& fbo, int width, int height)
{
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
  depthMap.SetParameterfv(GL_TEXTURE_BORDER_COLOR, array<float, 4>{ 1.0, 1.0, 1.0, 1.0 }.data());

  /* With the generated depth texture we can attach it as the framebuffer's depth buffer */
  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, depthMap.id, 0);
}
static void CreateDepthCubeMapFbo(FrameBuffer& fbo, int width, int height)
{
  fbo.Create();

  TextureCubemap texture;
  texture.Create();
  texture.CreateStorage(GL_DEPTH_COMPONENT24, width, height);
  for (int i = 0; i < 6; i++)
    texture.SubImage3D(0, 0, 0, i, width, height, 1, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  texture.SetParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  fbo.AttachTexture(GL_DEPTH_ATTACHMENT, texture.id, 0);
}

static void CreateDefaultTexture(Texture2D& texture, array<byte, 3> textureData)
{
  texture.Create();
  texture.CreateStorage(GL_RGB8, 1, 1);
  texture.UpdateStorage(0, 0, 0, GL_UNSIGNED_BYTE, textureData.data());
  texture.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
  /* ----------------- */
  g_windowManager.Initialize();
  CONSOLE_INFO("WindowManager ready");
  
  g_windowManager.SetWindowTitle("ProjectGL");
  g_windowManager.SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  g_windowManager.SetWindowPosition(50, 50);
  g_windowManager.SetWindowAspectRatio(16, 9);
  g_windowManager.SetWindowVsync(false);

  /* initialize viewport */
  /* ------------------- */
  _viewportSize.x = WINDOW_WIDTH;
  _viewportSize.y = WINDOW_HEIGHT;

  /* Setup ImGui context */
  /* ------------------- */
  ImGuiLayer::SetupContext();
  ImGuiLayer::SetFont((FONTS_PATH / "Karla-Regular.ttf"), 16);
  CONSOLE_INFO("ImGuiLayer ready");
  
  /* Initialize shaders */
  g_shaderManager.LoadShadersFromDir(SHADERS_PATH);
  g_shaderManager.LoadPrograms();
  g_shaderManager.SetUpProgramsUniforms();
  CONSOLE_INFO("ShaderManager ready!");

  /* Load textures */
  /* ------------- */
  /* !!The first 4 positions are reserved for the default textures */
  Texture2D defaultDiffuseTexture(GL_TEXTURE_2D);
  CreateDefaultTexture(defaultDiffuseTexture, array<byte, 3>{ byte(105), byte(105), byte(255) });
  Texture2D defaultSpecularTexture(GL_TEXTURE_2D);
  CreateDefaultTexture(defaultSpecularTexture, array<byte, 3>{ byte(255), byte(255), byte(255) });
  Texture2D defaultNormalTexture(GL_TEXTURE_2D);
  CreateDefaultTexture(defaultNormalTexture, array<byte, 3>{ byte(0), byte(0), byte(0) });
  Texture2D defaultHeightTexture(GL_TEXTURE_2D);
  CreateDefaultTexture(defaultHeightTexture, array<byte, 3>{ byte(0), byte(0), byte(0) });
  g_textureManager.LoadTexture(defaultDiffuseTexture);
  g_textureManager.LoadTexture(defaultSpecularTexture);
  g_textureManager.LoadTexture(defaultNormalTexture);
  g_textureManager.LoadTexture(defaultHeightTexture);
  g_textureManager.LoadTexturesFromDir(TEXTURES_PATH);
  CONSOLE_INFO("TextureManager ready!");

  /* Create Framebuffer object */
  CreateFramebuffer(4, _viewportSize.x, _viewportSize.y);
  CreateScreenSquare();

  /* Initialize uniform block objects */
  _uboCamera.target = GL_UNIFORM_BUFFER;
  _uboCamera.Create();
  _uboCamera.CreateStorage(2 * sizeof(mat4f), nullptr, GL_STATIC_DRAW);
  _uboCamera.BindBase(0); /* cameraBlock to bindingpoint 0 */

  SetOpenGLStates();
}
void Engine::Run()
{
  /* Create skybox object */
  TextureCubemap skyboxTexture;
  VertexArray skybox;
  CreateSkybox(skybox, skyboxTexture);

  /* Create scene object */
  Scene scene;
  scene.LoadScene((ROOT_PATH / "Scene.ini"));

  Components::DirectionalLight* directionalLight = nullptr;
  scene.Reg().view<Components::DirectionalLight>().each([&](auto& light) { directionalLight = &light; });

  Components::PointLight* pointLight = nullptr;
  scene.Reg().view<Components::PointLight>().each([&](auto& light) { pointLight = &light; });

  /* Create primary camera object */
  Camera primaryCamera(vec3f(0.0f, 5.0f, 10.0f));
  primaryCamera.frustum.zFar = 50.0f;

  /* Setting up the directional shadow mapping  */
  FrameBuffer fboDepthMap;
  CreateDepthMapFbo(fboDepthMap, 1024, 1024);

  Camera directLightCamera(vec3f(0.0f, 10.0f, 10.0f));
  directLightCamera.frustum.zFar = 30.0f;

  /* Setting up the omnidirectional shadow mapping */
  FrameBuffer fboDepthCubeMap;
  CreateDepthCubeMapFbo(fboDepthCubeMap, 1024, 1024);


  /* ---------------------------------------------------------------------- */
  /* -------------------------- Pre-loop section -------------------------- */
  /* ---------------------------------------------------------------------- */
  Program* framebufferProgram = g_shaderManager.GetProgram("Framebuffer");
  Program* sceneProgram = g_shaderManager.GetProgram("Scene");
  Program* sceneShadowsProgram = g_shaderManager.GetProgram("SceneShadows");
  Program* depthMapProgram = g_shaderManager.GetProgram("DepthMap");
  Program* depthCubeMapProgram = g_shaderManager.GetProgram("DepthCubeMap");
  Program* skyboxProgram = g_shaderManager.GetProgram("Skybox");
  uint32_t fboTexture = _fboIntermediate.GetTextureAttachment(0);
  uint32_t depthMapTexture = fboDepthMap.GetTextureAttachment(0);
  uint32_t depthCubeMapTexture = fboDepthCubeMap.GetTextureAttachment(0);

  auto lastUpdateTime = chrono::high_resolution_clock::now();
  bool renderingShadowsMode = false;
  bool useNormalMap = true;
  
  /* ------------------------------------------------------------------ */
  /* -------------------------- loop section -------------------------- */
  /* ------------------------------------------------------------------ */
  while (g_windowManager.IsOpen())
  {
    ImGuiLayer::BeginFrame();
    ImGuiLayer::Docking();

    /* ---------------------------------------------------------------------------------- */
    /* -------------------------- Per-frame time logic section -------------------------- */
    /* ---------------------------------------------------------------------------------- */
    float aspect = static_cast<float>(_viewportSize.x) / static_cast<float>(_viewportSize.y);
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = now - lastUpdateTime;
    double delta = diff.count();
    g_drawCalls = 0;

    /* ------------------------------------------------------------------- */
    /* -------------------------- Input section -------------------------- */
    /* ------------------------------------------------------------------- */
    g_windowManager.PoolEvents();
    primaryCamera.ProcessKeyboard(delta);
    primaryCamera.ProcessMouse(delta);

    if (g_windowManager.GetKey(GLFW_KEY_F1) == GLFW_PRESS) renderingShadowsMode = false;
    else if (g_windowManager.GetKey(GLFW_KEY_F2) == GLFW_PRESS) renderingShadowsMode = true;

    if (g_windowManager.GetKey(GLFW_KEY_F5) == GLFW_PRESS) useNormalMap = false;
    else if (g_windowManager.GetKey(GLFW_KEY_F6) == GLFW_PRESS) useNormalMap = true;

    /* -------------------------------------------------------------------- */
    /* -------------------------- Update section -------------------------- */
    /* -------------------------------------------------------------------- */
    primaryCamera.UpdateOrientation();
    mat4f cameraView = primaryCamera.CalculateView(primaryCamera.position + primaryCamera.GetFrontVector());
    mat4f cameraProj = primaryCamera.CalculatePerspective(aspect);
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

      array<mat4f, 6> pointLightViews{};
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

        sceneShadowsProgram->SetUniform1i("u_depthMapTexture", 10);
        sceneShadowsProgram->SetUniform1i("u_depthCubeMapTexture", 11);
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
      //{
      //  skyboxProgram->Use();
      //  skyboxProgram->SetUniformMat4f("u_projection", cameraProj);
      //  skyboxProgram->SetUniformMat4f("u_view", mat4f(mat3f(cameraView)));
      //  skyboxTexture.BindTextureUnit(0);
      //  Depth::SetFunction(GL_LEQUAL); 
      //  Renderer::DrawArrays(GL_TRIANGLES, skybox);
      //  Depth::SetFunction(GL_LESS); 
      //}

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

    //ImGuiLayer::RenderDemo();
    ImGuiLayer::RenderMenuBar(scene);
    ImGuiLayer::RenderDepthMap(depthMapTexture);
    ImGuiLayer::RenderCameraProps("Primary camera", primaryCamera);
    ImGuiLayer::RenderCameraProps("DirecLight camera", directLightCamera);
    ImGuiLayer::RenderGlobals();
    GameObject objectSelected = ImGuiLayer::RenderOutlinerPanel(scene);
    if (objectSelected.IsValid()) 
      ImGuiLayer::RenderDetails(objectSelected);
    vec2i32 viewport = ImGuiLayer::RenderViewportAndGuizmo(fboTexture, objectSelected, cameraView, cameraProj);
    ImGuiLayer::EndFrame();

    /* Checking viewport size */
    if (viewport != _viewportSize)
    {
      _viewportSize = viewport;
      _fboMultisampled.Delete();
      _fboIntermediate.Delete();
      CreateFramebuffer(4, viewport.x, viewport.y);
    }

    g_windowManager.SwapWindowBuffers();
    lastUpdateTime = now;
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

  g_shaderManager.CleanUp();
  g_textureManager.CleanUp();
  g_windowManager.CleanUp();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


void Engine::CreateFramebuffer(int samples, int width, int height)
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

  float vertices[] = {
    /* positions   texCoords */
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  Buffer vbo(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  _screenSquare.AttachVertexBuffer(0, vbo.id, 0, 4 * sizeof(float));

  _screenSquare.SetAttribFormat(0, 2, GL_FLOAT, true, 0);
  _screenSquare.SetAttribBinding(0, 0);
  _screenSquare.EnableAttribute(0);

  _screenSquare.SetAttribFormat(1, 2, GL_FLOAT, true, 2 * sizeof(float));
  _screenSquare.SetAttribBinding(1, 0);
  _screenSquare.EnableAttribute(1);
  
  _screenSquare.numVertices = 6;
  _screenSquare.numIndices = 0;
}
