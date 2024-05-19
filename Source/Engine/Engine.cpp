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

#include "Editor/Editor.hpp"

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

#if GUI_MODE
  editor = new Editor;
  editor->Initialize();
#endif
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

  VertexArray screenSquare;
  CreateScreenSquare(screenSquare);

  /* -------------------------- Framebuffer -------------------------- */
  Vec2i32 viewportSize = window.GetFramebufferSize();
  FrameBuffer framebufferMultisampled;
  FrameBuffer framebufferIntermediate;
  CreateFramebufferMultisampled(framebufferMultisampled, framebufferIntermediate, 4, viewportSize.x, viewportSize.y);
  const Texture2D& framebufferTexture = framebufferIntermediate.GetTextureAttachment(0);

  /* -------------------------- Camera -------------------------- */
  Camera camera(Vec3f(30.0f, 15.0f, 10.0f), 45.0f, static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y));
  camera.cameraComponent->yaw = -180.0f;
  camera.cameraComponent->pitch = -30.0f;
  camera.cameraComponent->roll = 0.0f;
  camera.cameraComponent->UpdateVectors();

  /* -------------------------- Scene -------------------------- */
  Scene scene;

  GameObject floor = scene.CreateObject();
  {
    auto& floorTransform = floor.AddComponent<TransformComponent>();
    auto& floorMesh = floor.AddComponent<StaticMeshComponent>((ASSETS_PATH / "Shapes/Plane/Plane.obj").lexically_normal());
    floorTransform.scale.x *= 20.0f;
    floorTransform.scale.z *= 10.0f;
    floorTransform.UpdateTransformation();
    floorMesh.material.diffuse = instanceTM.GetTextureByPath((TEXTURES_PATH / "wood.png").lexically_normal());
    floorMesh.material.specular = nullptr;
  }
  GameObject floor2 = scene.CreateObject();
  {
    auto& floorTransform = floor2.AddComponent<TransformComponent>();
    auto& floorMesh = floor2.AddComponent<StaticMeshComponent>((ASSETS_PATH / "Shapes/Plane/Plane.obj").lexically_normal());
    floorTransform.scale.x *= 20.0f;
    floorTransform.scale.z *= 10.0f;
    floorTransform.position.z = 20.0f;
    floorTransform.UpdateTransformation();
    floorMesh.material.diffuse = instanceTM.GetTextureByPath((TEXTURES_PATH / "wood.png").lexically_normal());
    floorMesh.material.specular = nullptr;
  }

  GameObject cubes[3];
  for (int i = 0; i < 3; i++)
  {
    cubes[i] = scene.CreateObject();
    auto& transform = cubes[i].AddComponent<TransformComponent>();
    transform.position.z = 15.0f;
    transform.position.x = i * 4;
    transform.position.y = 1;
    transform.UpdateTransformation();
    auto& mesh = cubes[i].AddComponent<StaticMeshComponent>((ASSETS_PATH / "Shapes/Cube/Cube.obj").lexically_normal());
    mesh.material.diffuse = instanceTM.GetTextureByPath((TEXTURES_PATH / "container_diffuse.png").lexically_normal());
    mesh.material.specular = instanceTM.GetTextureByPath((TEXTURES_PATH / "container_specular.png").lexically_normal());
  }

  GameObject dirLightObject = scene.CreateObject();
  {
    auto& directionalLight = dirLightObject.AddComponent<DirLightComponent>();
    directionalLight.color = { 1.0f,1.0f,1.0f };
    directionalLight.ambient = 0.0f;
    directionalLight.diffuse = 0.0f;
    directionalLight.specular = 1.0f;
    directionalLight.direction = { -0.2f, -1.0f, -0.3f };
  }

  GameObject pointLightObjects[4];
  {
    Vec3f lightPositions[4] = {
        Vec3f(-10.0f, 1.0f, 0.0f),
        Vec3f(-5.0f, 1.0f, 0.0f),
        Vec3f(0.0f, 1.0f, 0.0f),
        Vec3f(5.0f, 1.0f, 0.0f),
    };
    Vec3f lightColors[4] = {
        Vec3f(1, 0, 0),
        Vec3f(0, 1, 0),
        Vec3f(0, 1, 1),
        Vec3f(1, 1, 1),
    };

    for (int i = 0; i < 4; i++)
    {
      pointLightObjects[i] = scene.CreateObject();
      auto& pointLight = pointLightObjects[i].AddComponent<PointLightComponent>();
      pointLight.color = lightColors[i];
      pointLight.ambient = 0.0f;
      pointLight.diffuse = 1.0f;
      pointLight.specular = 1.0f;
      pointLight.position = lightPositions[i];
    }
  }

  /* -------------------------- Shadow mapping -------------------------- */
  //FrameBuffer framebufferShadowMap;
  //CreateShadowMapFramebuffer(framebufferShadowMap, 1024, 1024);
  //const Texture2D& DepthMapTexture = framebufferShadowMap.GetTextureAttachment(0);

  /* -------------------------- Pre-loop -------------------------- */
  SetShadersUniforms();
  SetGlobalGlStates();
  glClearColor(0.15, 0.15, 0.15, 1.0f);
  glClearDepth(1.0f);
  glClearStencil(0);

  time_point lastUpdateTime = system_clock::now();

  
  /* -------------------------- loop -------------------------- */
  while (window.IsOpen())
  {
#if GUI_MODE
    editor->Begin();
#endif

    /* -------------------------- Per-frame time logic -------------------------- */
    const auto now = system_clock::now();
    const std::chrono::duration<double> diff = now - lastUpdateTime;
    const double delta = diff.count();
    drawCalls = 0;

    /* -------------------------- Input -------------------------- */
    window.PoolEvents();


#if GUI_MODE
    if (editor->viewportPanel->isFocused)
      camera.ProcessInput(window, delta);
#else
    camera.ProcessInput(window, delta);
#endif

    /* -------------------------- Update -------------------------- */
    const auto& cameraViewMatrix = camera.cameraComponent->GetView();
    const auto& cameraProjectionMatrix = camera.cameraComponent->GetProjection();
    const auto time = glfwGetTime();


    //const Mat4f lightProjection = Math::Ortho(
    //  -10.0f, 10.0f, -10.0f, 10.0f, INITIAL_ZNEAR, INITIAL_ZFAR);
    //Mat4f lightView = Math::LookAt(
    //  lightPosition,
    //  Vec3f(0.0f, 0.0f, 0.0f), // Vec3f(0.0f, 0.0f, 0.0f) | dirLight->direction 
    //  Vec3f(0.0f, 1.0f, 0.0f)
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

        auto dirlight = dirLightObject.GetComponent<DirLightComponent>();
        sceneProgram->SetUniform3f("u_directionalLight.color", dirlight->color);
        sceneProgram->SetUniform1f("u_directionalLight.ambient", dirlight->ambient);
        sceneProgram->SetUniform1f("u_directionalLight.diffuse", dirlight->diffuse);
        sceneProgram->SetUniform1f("u_directionalLight.specular", dirlight->specular);
        sceneProgram->SetUniform3f("u_directionalLight.direction", dirlight->direction);

        for (int i = 0; i < 4; i++)
        {
          auto* light = pointLightObjects[i].GetComponent<PointLightComponent>();
          sceneProgram->SetUniform3f(std::format("u_pointLight[{}].color", i).c_str(), light->color);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].ambient", i).c_str(), light->ambient);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].diffuse", i).c_str(), light->diffuse);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].specular", i).c_str(), light->specular);
          sceneProgram->SetUniform3f(std::format("u_pointLight[{}].position", i).c_str(), light->position);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].linear", i).c_str(), light->linear);
          sceneProgram->SetUniform1f(std::format("u_pointLight[{}].quadratic", i).c_str(), light->quadratic);
        }
        
        sceneProgram->SetUniformMat4f("u_model", floor.GetComponent<TransformComponent>()->GetTransformation());
        floor.GetComponent<StaticMeshComponent>()->Draw();
        sceneProgram->SetUniformMat4f("u_model", floor2.GetComponent<TransformComponent>()->GetTransformation());
        floor2.GetComponent<StaticMeshComponent>()->Draw();

        Culling::EnableFaceCulling();
        for (int i = 0; i < 3; i++)
        {
          auto cubeTransform = cubes[i].GetComponent<TransformComponent>();
          auto mesh = cubes[i].GetComponent<StaticMeshComponent>();
          sceneProgram->SetUniformMat4f("u_model", cubeTransform->GetTransformation());
          mesh->Draw();
        }
        Culling::DisableFaceCulling();
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

#if !GUI_MODE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebufferProgram->Use();
    framebufferTexture.BindTextureUnit(0);
    DrawArrays(GL_TRIANGLES, screenSquare);

#else
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
#endif

    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
}

void Engine::CleanUp()
{
  ShaderManager::Instance().CleanUp();
  
  TextureManager::Instance().CleanUp();

#if GUI_MODE
  editor->CleanUp();
  delete editor;
#endif

  WindowManager::Instance().CleanUp();
}
