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

  /* Load shaders */
  ShadersManager::Initialize();
  auto framebufferShader = ShadersManager::GetShader("FramebufferShader");
  framebufferShader->Use();
  framebufferShader->SetInt("screenTexture", 0);
  auto instancingShader = ShadersManager::GetShader("InstancingShader");
  instancingShader->Use();
  instancingShader->SetInt("Material.diffuse", 0);
  instancingShader->SetInt("Material.specular", 1);
  instancingShader->SetFloat("Material.shininess", 32.0f);
  auto sceneShader = ShadersManager::GetShader("SceneShader");
  sceneShader->Use();
  sceneShader->SetInt("Material.diffuse", 0);
  sceneShader->SetInt("Material.specular", 1);
  sceneShader->SetFloat("Material.shininess", 32.0f);
  
  /* Load textures from Textures directory */
  TexturesManager::Initialize();

  
}

void Engine::Run()
{
  auto testingShader = ShadersManager::GetShader("TestingShader");
  auto instancingShader = ShadersManager::GetShader("InstancingShader");
  auto framebufferShader = ShadersManager::GetShader("FramebufferShader");
  auto sceneShader = ShadersManager::GetShader("SceneShader");

  /* Window object */
  Window window(glfwGetCurrentContext());

  /* Initialize framebuffer object */
  FrameBuffer framebuffer;
  framebuffer.InitializeFrameBuffer(window.GetFramebufferSize());

  /* Camera object */
  Camera camera(window.GetWindowSize(), Vec3f(0.0f, 0.0f, 10.0f));
  
  /* Mesh objects */
  StaticMesh cube("Shapes/Cube/Cube.obj");
  cube.position.x = -2.5f;
  cube.GetMesh(0)->diffuse = nullptr;

  InstancingMesh instancingcube(cube.GetMesh(0), 10);
  instancingcube.diffuse = TexturesManager::GetTexture("default.png");
  instancingcube.AddInstance(Mat4f(1.0f));
  instancingcube.AddInstance(glm::translate(Mat4f(1.0f), Vec3f(0.0f, 2.5f, 0.0f)));

  /* Lighting */
  DirectionalLight dirLight("DirLight");
  dirLight.ambient = 0.1f;
  dirLight.diffuse = 0.25f;
  dirLight.specular = 0.75f;
  //PointLight pointLight("PointLight");
  //pointLight.position = Vec3f(0.0f, 5.0f, 0.0f);
  
  /* Create scene */
  //Scene scene;
  //scene.directionalLight = &dirLight;
  //scene.AddPointLight(&pointLight);
  //scene.AddStaticMesh(&plane);

  /* Pre-loop */
  double lastUpdateTime = 0;
  Vec2i windowFbSize = window.GetFramebufferSize();
  float aspectRatio = ((float)windowFbSize.x / (float)windowFbSize.y);

  /* Loop  */
  while (window.Loop())
  {
    editor.NewFrame();

    /* Per - frame time logic */
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

    /* input */
    glfwPollEvents();
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
      window.CloseWindow();
    camera.ProcessInput(&window, deltaTime);
    const Mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    const Mat4f view = camera.GetViewMatrix();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    framebuffer.BindMSAAFramebuffer();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    /* Render scene */
    instancingShader->Use();
    instancingShader->SetMat4f("Projection", projection);
    instancingShader->SetMat4f("View", view);
    dirLight.RenderLight(instancingShader);
    instancingcube.Draw();

    sceneShader->Use();
    sceneShader->SetMat4f("Projection", projection);
    sceneShader->SetMat4f("View", view);
    dirLight.RenderLight(sceneShader);
    cube.Draw(sceneShader);

    framebuffer.BlitFrameBuffer();
    framebuffer.UnbindFrameBuffer();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    framebufferShader->Use();
    framebuffer.DrawFrame(framebufferShader);

    /* Render editor */
    //editor.MenuBar();
    //editor.ShowDemo();
    //editor.ShowScenePanel(&scene);
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);          // enable 4x MSAA on GLFW frame buffer

  GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y, "OpenGL", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowPos(window, 250, 50);        // set default position
  glfwSetWindowAspectRatio(window, 16, 9);  // aspect ratio 16:9
  //glfwSwapInterval(1);                      // v-sync on
  //glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowResized = true;
  });
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
  });
  
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  /* antialising */
  glEnable(GL_MULTISAMPLE);
  
  /* depth buffer */
  glEnable(GL_DEPTH_TEST);
  
  /* blending */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  /* face culling */
  glDisable(GL_CULL_FACE);
}


