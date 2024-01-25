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


static bool windowResized = false;

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

  /* Initialize Editor */
  editor.Initialize();

  /* Load and initialize shaders */
  ShadersManager::Initialize();
  
  /* Load textures from Textures directory */
  TexturesManager::Initialize();
}

void Engine::Run()
{
  auto testingShader = ShadersManager::GetShader("TestingShader");
  auto instancingShader = ShadersManager::GetShader("InstancingShader");
  auto framebufferShader = ShadersManager::GetShader("FramebufferShader");
  auto sceneShader = ShadersManager::GetShader("SceneShader");
  auto shadowMapDepthShader = ShadersManager::GetShader("ShadowMapDepthShader");
  auto shadowMapShader = ShadersManager::GetShader("ShadowMapShader");

  /* Window object */
  Window window(glfwGetCurrentContext());

  /* Initialize framebuffer object */
  FrameBuffer framebuffer;
  framebuffer.InitFrameBuffer(window.GetFramebufferSize());

  /* Camera object */
  Camera camera(window.GetWindowSize(), Vec3f(0.0f, 1.0f, 10.0f));
  
  /* Mesh objects */
  StaticMesh plane("Shapes/Plane/Plane.obj");
  plane.scaling *= 10.f;
  StaticMesh cube1("Shapes/Cube/Cube.obj");
  cube1.position = Vec3f(0.0f, 4.0f, 0.0f);
  cube1.scaling *= 0.75f;
  StaticMesh cube2("Shapes/Cube/Cube.obj");
  cube2.position = Vec3f(0.0f, 1.0f, 2.0f);
  cube2.scaling *= 0.75f;
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
  //PointLight pointLight("UPointLight");
  //pointLight.position = Vec3f(0.0f, 5.0f, 0.0f);
  
  /* Create scene */
  Scene scene;
  //scene.directionalLight = &dirLight;
  //scene.AddStaticMesh(&cube1);
  //scene.AddStaticMesh(&cube2);
  //scene.AddStaticMesh(&plane);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); /* Attach 2D texture as the framebuffer's depth buffer*/
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
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
  VertexArray screenQuad;
  screenQuad.InitVertexArray(data, config);

  testingShader->Use();
  testingShader->SetInt("UShadowMap", 0);
  shadowMapShader->Use();
  shadowMapShader->SetInt("UDiffuseTexture", 0);
  shadowMapShader->SetInt("USpecularTexture", 1);
  shadowMapShader->SetInt("UShadowMap", 2);


  /* Pre-loop */
  double lastUpdateTime = 0;
  Vec2i windowFbSize = window.GetFramebufferSize();
  float aspectRatio = ((float)windowFbSize.x / (float)windowFbSize.y);

  /* Loop  */
  while (window.Loop())
  {
    editor.NewFrame();

    /* Per-frame time logic */
    const double now = glfwGetTime();
    const double deltaTime = now - lastUpdateTime;
    Renderer::drawCalls = 0;
    if (windowResized)
    {
      windowFbSize = window.GetFramebufferSize();
      aspectRatio = ((float)windowFbSize.x / (float)windowFbSize.y);
      framebuffer.RescaleFrameBuffer(windowFbSize);
      windowResized = false;
    }

    /* Input */
    glfwPollEvents();
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
      window.CloseWindow();
    camera.ProcessInput(&window, deltaTime);

    
    /* Render */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render depth of scene to texture (from light's perspective) */
    float near_plane = 1.0f;
    float far_plane = 7.5f;
    Vec3f lightPos = Vec3f(-2.0f, 6.0f, -1.0f);
    Mat4f lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    Mat4f lightView = glm::lookAt(lightPos, Vec3f(0.0f), Vec3f(0.0f, 1.0f, 0.0f));
    Mat4f lightSpaceMatrix = lightProjection * lightView;
    shadowMapDepthShader->Use();
    shadowMapDepthShader->SetMat4f("ULightSpaceMatrix", lightSpaceMatrix);
    
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    plane.Draw(shadowMapDepthShader);
    cube1.Draw(shadowMapDepthShader);
    cube2.Draw(shadowMapDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Reset viewport */
    glViewport(0, 0, windowFbSize.x, windowFbSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Render scene as normal using the generated shadow map */
    const Mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    const Mat4f view = camera.GetViewMatrix();
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

    /* Render Depth map to quad for visual debugging */
    //testingShader->Use();
    //testingShader->SetFloat("UNearPlane", near_plane);
    //testingShader->SetFloat("UFarPlane", far_plane);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, depthMap);
    //Renderer::drawMode = GL_TRIANGLE_STRIP;
    //Renderer::DrawArrays(&screenQuad);
    //Renderer::drawMode = GL_TRIANGLES;

    {
      /* Bind frame buffer */
      //framebuffer.BindMSAAFramebuffer();
      //glClearColor(pow(0.1f, 2.2f), pow(0.1f, 2.2f), pow(0.1f, 2.2f), 1.0f);
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //glEnable(GL_DEPTH_TEST);

      /* Render scene to frame buffer texture */
      //instancingShader->Use();
      //instancingShader->SetMat4f("UProjection", projection);
      //instancingShader->SetMat4f("UView", view);
      //dirLight.RenderLight(instancingShader);
      //instancingPlane.DrawInstMesh();
      //sceneShader->Use();
      //sceneShader->SetMat4f("UProjection", projection);
      //sceneShader->SetMat4f("UView", view);
      //scene.DrawScene(sceneShader);

      /* Draw frame buffer image */
      //framebuffer.BlitFrameBuffer();
      //framebuffer.UnbindFrameBuffer();
      //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      //glClear(GL_COLOR_BUFFER_BIT);
      //glDisable(GL_DEPTH_TEST);
      //framebufferShader->Use();
      //framebuffer.DrawFrame(framebufferShader);
    }
    /* Render editor */
    //editor.MenuBar();
    //editor.ShowDemo();
    editor.ShowScenePanel(&scene);
    editor.ShowStats();
    editor.RenderFrame();
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
  ShadersManager::ShutDown();
  TexturesManager::ShutDown();

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
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* OpenGL 4.6 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);          /* enable 4x MSAA on GLFW frame buffer */ 

  GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y, "OpenGL", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowPos(window, 250, 50);        /* set default position */ 
  glfwSetWindowAspectRatio(window, 16, 9);  /* aspect ratio 16:9 */
  //glfwSwapInterval(1);                    /* v-sync on */
  //glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowResized = true;
  });
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

