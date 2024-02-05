#include "Engine.hh"
#include "Logger.hh"
#include "Window.hh"
#include "Camera.hh"
#include "Shader.hh"
#include "Scene.hh"
#include "Renderer.hh"
#include "FrameBuffer.hh"

#include "Mesh/StaticMesh.hh"
#include "Mesh/InstancingMesh.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

#include "Subsystems/ShadersManager.hh"
#include "Subsystems/TexturesManager.hh"
#include "Subsystems/ConfigurationsManager.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  /* Init Logger */
  Logger::Initialize();

  /* Load configuration */
  ConfigurationsManager::Instance().Initialize();

  /* Initialize GLFW, create window and initialize OpenGL context */
  InitOpenGL();

  /* Load and initialize shaders */
  ShadersManager::Instance().Initialize();
  LoadShaders();
  
  /* Load textures from Textures directory */
  TexturesManager::Instance().Initialize();
  LoadTextures();

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

  /* Window object */
  Window window(glfwGetCurrentContext());

  /* Initialize framebuffer object */
  FrameBuffer framebuffer;
  framebuffer.InitFrameBuffer(editor.GetViewportSize());
  glViewport(0, 0, framebuffer.GetSize().x, framebuffer.GetSize().y);

  /* Camera object */
  Camera camera(window.GetWindowSize(), Vec3f(0.0f, 1.0f, 10.0f));
  
  /* Mesh objects */
  StaticMesh plane("Shapes/Plane/Plane.obj");
  plane.scaling *= 10.f;
  StaticMesh cube1("Shapes/Cube/Cube.obj");
  cube1.position = Vec3f(0.0f, 2.0f, 0.0f);
  cube1.scaling *= 0.5f;
  StaticMesh cube2("Shapes/Cube/Cube.obj");
  cube2.position = Vec3f(0.0f, 0.51f, 1.0f);
  cube2.scaling *= 0.5f;
  //StaticMesh cottage("Cottage/Cottage.obj");

  //InstancingMesh instancingPlane(&plane, 10);
  //instancingPlane.AddInstance(glm::translate(Mat4f(1.0f), Vec3f(0.0f, 0.0f, 0.0f)));
  //instancingPlane.AddInstance(glm::translate(Mat4f(1.0f), Vec3f(2.0f, 0.0f, 0.0f)));
  //instancingPlane.AddInstance(glm::translate(Mat4f(1.0f), Vec3f(0.0f, 0.0f, 2.0f)));
  //instancingPlane.AddInstance(glm::translate(Mat4f(1.0f), Vec3f(2.0f, 0.0f, 2.0f)));

  /* Lighting */
  DirectionalLight dirLight("UDirLight");
  dirLight.ambient = 0.1f;
  dirLight.diffuse = 0.25f;
  dirLight.specular = 0.75f;
  PointLight pointLight1("UPointLight[0]");
  pointLight1.position = Vec3f(0.0f, 5.0f, 0.0f);
  PointLight pointLight2("UPointLight[1]");
  pointLight2.position = Vec3f(0.0f, 5.0f, 5.0f);
  
  /* Create scene */
  Scene scene;
  scene.directionalLight = &dirLight;
  scene.AddStaticMesh(&cube1);
  scene.AddStaticMesh(&cube2);
  scene.AddStaticMesh(&plane);
  scene.AddPointLight(&pointLight1);
  scene.AddPointLight(&pointLight2);

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
    editor.NewFrame();

    /* Per-frame time logic */
    const double now = glfwGetTime();
    const double deltaTime = now - lastUpdateTime;
    Renderer::drawCalls = 0;

    /* Input */
    glfwPollEvents();
    if (editor.ViewportFocused())
      camera.ProcessInput(&window, deltaTime);
    
    const Vec2i framebufferSize = framebuffer.GetSize();
    const float aspectRatio = ((float)framebufferSize.x / (float)framebufferSize.y);
    const Mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, perspectiveDistance.x, perspectiveDistance.y);
    const Mat4f view = camera.GetViewMatrix();
    
    /* Render: bind frame buffer */
    framebuffer.BindMSAAFramebuffer();
    glClearColor(pow(0.1f, GAMMA_CORRECTION), pow(0.1f, GAMMA_CORRECTION), pow(0.1f, GAMMA_CORRECTION), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    sceneShader->Use();
    sceneShader->SetMat4f("UProjection", projection);
    sceneShader->SetMat4f("UView", view);
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
    editor.RenderFrame(&scene, &framebuffer);
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
  auto& instanceCM = ConfigurationsManager::Instance();
  /* Get window size */
  String& windowRes = instanceCM.GetValue(CONF_WINDOW_RESOLUTION);
  Vec2i resolution = instanceCM.ParseResolution(windowRes);
  /* Get window pos */
  String& windowPos = instanceCM.GetValue(CONF_WINDOW_POS);
  Vec2i position = instanceCM.ParsePosition(windowPos);
  /* Get window title */
  String& title = instanceCM.GetValue(CONF_WINDOW_TITLE);
  /* Get window aspect ratio */
  String& windowAR = instanceCM.GetValue(CONF_ASPECT_RATIO);
  Vec2i aspectRatio = instanceCM.ParseAspectRatio(windowAR);
  /* V-sync on-off */
  String& vsync = instanceCM.GetValue(CONF_VSYNC);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  /* Load OpenGL 4.6 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowPos(window, position.x, position.y); /* Set window position */
  glfwSetWindowAspectRatio(window, aspectRatio.x, aspectRatio.y);  /* Set aspect ratio */
  glfwSwapInterval((std::strcmp(vsync.c_str(), "true") == 0 ? 1 : 0)); /* V-sync */
  
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
}

void Engine::LoadTextures()
{
  for (auto& entry : std::filesystem::recursive_directory_iterator(ROOT_PATH / "Textures"))
    if (!std::filesystem::is_directory(entry))
      TexturesManager::Instance().LoadTexture(entry);
  
  for (auto& entry : std::filesystem::recursive_directory_iterator(ROOT_PATH / "Icons"))
    if (!std::filesystem::is_directory(entry))
      TexturesManager::Instance().LoadTexture(entry);
}
