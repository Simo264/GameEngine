#include "Engine.hpp"

#include "Logger.hpp"
#include "Engine/Window.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"
#include "Engine/StaticMesh.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"

#include "Engine/Subsystems/ShadersManager.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"

#include "INIFileManager.hpp"

#define GAMMA_CORRECTION 2.2f

Mat4f cameraProjectionMatrix;
Mat4f cameraViewMatrix;


/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  /* Init Logger */
  Logger::Initialize();

  /* Initialize GLFW, create window and initialize OpenGL context */
  InitOpenGL();

  /* Load and initialize shaders */
  ShadersManager::Instance().Initialize();
  LoadShaders();
  
  /* Load textures from Textures directory */
  TexturesManager::Instance().Initialize();
  LoadTextures();
  LoadIcons();

  /* Initialize ImGui library */
  editor.Initialize();
}

void Engine::Run()
{
  auto& instanceSM          = ShadersManager::Instance();
  auto testingShader        = instanceSM.GetShader("TestingShader");
  auto instancingShader     = instanceSM.GetShader("InstancingShader");
  auto framebufferShader    = instanceSM.GetShader("FramebufferShader");
  auto sceneShader          = instanceSM.GetShader("SceneShader");
  auto shadowMapDepthShader = instanceSM.GetShader("ShadowMapDepthShader");
  auto shadowMapShader      = instanceSM.GetShader("ShadowMapShader");

  /* Initialize framebuffer object */
  Window window(glfwGetCurrentContext());

  FrameBuffer framebuffer;
  framebuffer.InitFrameBuffer(window.GetFramebufferSize());
  glViewport(0, 0, framebuffer.GetSize().x, framebuffer.GetSize().y);

  /* Camera object */
  Camera camera(Vec3f(0.0f, 1.0f, 10.0f), 45.0f);
  
  /* Create scene */
  Scene scene;
  scene.LoadScene(ROOT_PATH / "scene.ini");

#if 0
  /* Shadow mapping */
  uint32_t SHADOW_WIDTH = 1024;
  uint32_t SHADOW_HEIGHT = 1024;
  uint32_t depthMapFBO; /* Framebuffer object for rendering the depth map */
  uint32_t depthMap;    /* Create a 2D texture that we'll use as the framebuffer's depth buffer */
  {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); /* Attach 2D texture as the framebuffer's depth buffer*/
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  VertexArray screenQuad;
  {
    float vertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    VertexArrayData data;
    data.vertDataSize = sizeof(vertices);
    data.vertData = vertices;
    VertexArrayConfig config;
    config.PushAttributes({ 3, 2 });

    screenQuad.InitVertexArray(data, config);
  }

  testingShader->Use();
  testingShader->SetInt("UShadowMap", 0);
  shadowMapShader->Use();
  shadowMapShader->SetInt("UDiffuseTexture", 0);
  shadowMapShader->SetInt("USpecularTexture", 1);
  shadowMapShader->SetInt("UShadowMap", 2);
#endif

  /* Pre-loop */
  double lastUpdateTime = 0;
  const Vec2f perspectiveDistance(0.1f, 100.0f); 
  //Vec3f lightPos = Vec3f(-2.0f, 10.0f, -2.0f); /* Shadows */
  //Mat4f lightSpaceMatrix; /* Shadows */

  /* Loop  */
  while (window.Loop())
  {
    editor.Begin();

    /* Per-frame time logic */
    const double now = glfwGetTime();
    const double deltaTime = now - lastUpdateTime;
    Renderer::drawCalls = 0;

    /* Input */
    glfwPollEvents();
    if (editor.viewportPanel->isFocused)
      camera.ProcessInput(deltaTime);
    
    const Vec2i framebufferSize = framebuffer.GetSize();
    const float aspectRatio = ((float)framebufferSize.x / (float)framebufferSize.y);
    cameraProjectionMatrix = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    cameraViewMatrix = camera.GetViewMatrix();
    
    /* Render: bind frame buffer */
    framebuffer.BindMSAAFramebuffer();
    glClearColor(pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), pow(0.3f, GAMMA_CORRECTION), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    sceneShader->Use();
    sceneShader->SetMat4f("UProjection", cameraProjectionMatrix);
    sceneShader->SetMat4f("UView", cameraViewMatrix);
    scene.DrawScene(sceneShader);
    
    framebuffer.BlitFrameBuffer();
    framebuffer.UnbindFrameBuffer();

#if 0
    /* Fill the depth map framebuffer (from light's perspective) */
    {
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      Mat4f lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
      Mat4f lightView = glm::lookAt(lightPos, Vec3f(0.0f), Vec3f(0.0f, 1.0f, 0.0f));
      lightSpaceMatrix = lightProjection * lightView;
      shadowMapDepthShader->Use();
      shadowMapDepthShader->SetMat4f("ULightSpaceMatrix", lightSpaceMatrix);
      plane.Draw(shadowMapDepthShader);
      glCullFace(GL_FRONT); /* To fix peter panning we cull all front faces during the shadow map generation */
      cube1.Draw(shadowMapDepthShader);
      cube2.Draw(shadowMapDepthShader);
      glCullFace(GL_FRONT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      framebuffer.BindMSAAFramebuffer();
    }


    /* Reset viewport */
    glViewport(0, 0, windowFbSize.x, windowFbSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render scene as normal using the generated shadow map */
    {

      shadowMapShader->Use();
      shadowMapShader->SetMat4f("UView", view);
      shadowMapShader->SetMat4f("UProjection", projection);
      shadowMapShader->SetMat4f("ULightSpaceMatrix", lightSpaceMatrix);
      shadowMapShader->SetVec3f("ULightPos", lightPos);
      shadowMapShader->SetVec3f("UViewPos", camera.position);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      plane.Draw(shadowMapShader);
      cube1.Draw(shadowMapShader);
      cube2.Draw(shadowMapShader);
    }

    /* Draw frame buffer image to editor Viewport */
    framebuffer.BlitFrameBuffer();
    framebuffer.UnbindFrameBuffer();

    const double time = glfwGetTime();
    lightPos.x = sin(time) * 10.f;
    lightPos.z = cos(time) * 10.f;

    /* Render Depth map to quad for visual debugging */
    {
      testingShader->Use();
      testingShader->SetFloat("UNearPlane", near_plane);
      testingShader->SetFloat("UFarPlane", far_plane);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      Renderer::drawMode = GL_TRIANGLE_STRIP;
      Renderer::DrawArrays(&screenQuad);
      Renderer::drawMode = GL_TRIANGLES;
    }
#endif

    /* Render editor */
    editor.Render(&scene, &framebuffer);
    editor.End();
    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
  
    /* Destroy framebuffer */
    framebuffer.DestroyFrameBuffer();
}

void Engine::ShutDown()
{
  /* Destroy all meshes */
  //for(auto mesh : meshes)
  //  mesh->Destroy();

  /* Shutdown subsystems */
  ShadersManager::Instance().ShutDown();
  TexturesManager::Instance().ShutDown();

  /* Shutdown ImGui */
  editor.ShutDown();

  /* GLFW: terminate, clearing all previously allocated GLFW resources */
  GLFWwindow* window = glfwGetCurrentContext();
  glfwDestroyWindow(window);
  glfwTerminate();
}



/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Engine::InitOpenGL()
{
  INIFileManager conf(ROOT_PATH / CONFIG_FILENAME);
  conf.ReadData();

  String title = conf.GetValue("window", "title");

  String tmp;
  tmp.reserve(32);
  tmp = conf.GetValue("window", "resolution");
  Vec2i resolution = INIFileManager::StringToVec2<Vec2i>(tmp, 'x');
  tmp = conf.GetValue("window", "aspectratio");
  Vec2i aspectratio = INIFileManager::StringToVec2<Vec2i>(tmp, ':');
  tmp = conf.GetValue("window", "position");
  Vec2i position = INIFileManager::StringToVec2<Vec2i>(tmp, ',');
  tmp = conf.GetValue("window", "vsync");
  bool vsync = INIFileManager::StringToBool(tmp);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  /* Load OpenGL 4.6 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowPos(window, position.x, position.y); /* Set window position */
  glfwSetWindowAspectRatio(window, aspectratio.x, aspectratio.y);  /* Set aspect ratio */
  glfwSwapInterval(vsync); /* V-sync */
  
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
  });

  /* Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error. */
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    CONSOLE_ERROR("Failed to initialize OpenGL context");
    exit(-1);
  }
  
  /* Antialising */
  glEnable(GL_MULTISAMPLE);
  
  /* Depth buffer */
  glEnable(GL_DEPTH_TEST);
  
  /* Blending */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  /* Face culling */
  glDisable(GL_CULL_FACE);

  /* Gamma correction */
  //glEnable(GL_FRAMEBUFFER_SRGB);
}

void Engine::LoadShaders()
{
  auto& instanceSM = ShadersManager::Instance();
  const Path shadersDir = ROOT_PATH / "Shaders";
  auto testingShader = instanceSM.LoadShaderProgram(
    "TestingShader",
    shadersDir / "Testing.vert",
    shadersDir / "Testing.frag");
  auto instancingShader = instanceSM.LoadShaderProgram(
    "InstancingShader",
    shadersDir / "Instancing.vert",
    shadersDir / "Scene.frag");
  auto sceneShader = instanceSM.LoadShaderProgram(
    "SceneShader",
    shadersDir / "Scene.vert",
    shadersDir / "Scene.frag");
  auto framebufferShader = instanceSM.LoadShaderProgram(
    "FramebufferShader",
    shadersDir / "Framebuffer.vert",
    shadersDir / "Framebuffer.frag");
  auto shadowMapDepthShader = instanceSM.LoadShaderProgram(
    "ShadowMapDepthShader",
    shadersDir / "ShadowMapDepth.vert",
    shadersDir / "ShadowMapDepth.frag");
  auto shadowMapShader = instanceSM.LoadShaderProgram(
    "ShadowMapShader",
    shadersDir / "ShadowMap.vert",
    shadersDir / "ShadowMap.frag");

  framebufferShader->Use();
  framebufferShader->SetInt("UScreenTexture", 0);
  framebufferShader->SetInt("UPostProcessingType", 0);

  instancingShader->Use();
  instancingShader->SetInt("UMaterial.diffuse", 0);
  instancingShader->SetInt("UMaterial.specular", 1);
  instancingShader->SetFloat("UMaterial.shininess", 32.0f);
  instancingShader->SetFloat("UGamma", 2.2f);

  sceneShader->Use();
  sceneShader->SetInt("UMaterial.diffuse", 0);
  sceneShader->SetInt("UMaterial.specular", 1);
  sceneShader->SetFloat("UMaterial.shininess", 32.0f);
  sceneShader->SetFloat("UGamma", 2.2f);
  sceneShader->SetFloat("UDirLightVisible", true);
  sceneShader->SetIntArray("UPointLightVisible", 5, Array<int, 5>{1,1,1,1,1}.data());
}

void Engine::LoadTextures()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(ROOT_PATH / "Textures"))
    if (!std::filesystem::is_directory(entry))
      TexturesManager::Instance().LoadTexture(entry);
}

void Engine::LoadIcons()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(ROOT_PATH / "Icons"))
    if (!std::filesystem::is_directory(entry))
      TexturesManager::Instance().LoadTexture(entry);
}
