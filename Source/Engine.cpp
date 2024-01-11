#include "Engine.hh"
#include "Logger.hh"
#include "Window.hh"
#include "Camera.hh"
#include "Scene.hh"

#include "Mesh/Shapes/Cube.hh"
#include "Mesh/Shapes/Plane.hh"
#include "Mesh/Shapes/Cylinder.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

#include "Subsystems/ShadersManager.hh"
#include "Subsystems/TexturesManager.hh"

#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"
#include "Graphics/FrameBuffer.hh"

static bool windowResized = false;

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Engine::Initialize()
{
  // Init Logger
  // ---------------------------------------
  Logger::Initialize();

  // init libraries and create GLFW window
  // ---------------------------------------
  InitOpenGL();

  // Initialize Editor
  // ---------------------------------------
  editor.Initialize();

  // Load shaders 
  // ---------------------------------------
  LoadShaders();
  auto instancingShader = ShadersManager::GetShader("InstancingShader");
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);
  auto framebufferShader = ShadersManager::GetShader("FramebufferShader");
  framebufferShader->Use();
  framebufferShader->SetInt("screenTexture", 0);
  auto sceneShader = ShadersManager::GetShader("SceneShader");
  sceneShader->Use();
  sceneShader->SetInt("Material.diffuse", 0);
  sceneShader->SetInt("Material.specular", 1);
  sceneShader->SetFloat("Material.shininess", 32.0f);
  
  // Load textures from Textures directory
  // ---------------------------------------
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

  /* Camera object */
  Camera camera(window.GetWindowSize(), Vec3f(0.0f, 0.0f, 10.0f));

  /* Mesh objects */ 
  Cube cube;

  /* Lighting */
  DirectionalLight dirLight("DirLight");
  dirLight.ambient = 0.1f;
  dirLight.diffuse = 0.2f;
  dirLight.specular = 0.0f;
  PointLight pointLight("PointLight");
  pointLight.position = Vec3f(0.0f, 2.0f, 0.0f);
  
  /* Create scene */
  Scene scene;
  scene.directionalLight = &dirLight;
  scene.AddStaticMesh(&cube);

  /* FrameBuffer object */
  FrameBuffer framebuffer(window.GetFramebufferSize());


  /* Loop  */
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    editor.NewFrame();

    /* Per - frame time logic */
    const double now = glfwGetTime();
    const double deltaTime = now - lastUpdateTime;
    const Vec2i windowFbSize = window.GetFramebufferSize();
    const float aspectRatio = (float)(windowFbSize.x / windowFbSize.y);
    
    Renderer::drawCalls = 0;


    /* input */
    glfwPollEvents();
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
      window.CloseWindow();
    camera.ProcessInput(&window, deltaTime);
    const Mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    const Mat4f view = camera.GetViewMatrix();
    sceneShader->Use();
    sceneShader->SetMat4f("Projection", projection);
    sceneShader->SetMat4f("View", view);

    if (windowResized)
    {
      CONSOLE_TRACE("Resizing framebuffer...");
      framebuffer.RescaleFrameBuffer(windowFbSize);
      CONSOLE_TRACE("Done");
      windowResized = false;
    }

    /* Render scene */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    framebuffer.BindMSAAFramebuffer();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
 
    scene.DrawScene(sceneShader);

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
    editor.ShowScenePanel(&scene);
    editor.ShowStats();
    editor.RenderFrame();
    window.SwapWindowBuffers();
    lastUpdateTime = now;
  }
}

void Engine::ShutDown()
{
  /* Shutdown Editor */
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
  glfwWindowHint(GLFW_SAMPLES, 4);          // enable 4x MSAA on GLFW framebuffer

  GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_SIZE_X, INITIAL_WINDOW_SIZE_Y, "OpenGL", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowAspectRatio(window, 16, 9);  // aspect ratio 16:9
  glfwSetWindowPos(window, 300, 50);        // set default position
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
  
  /* blending / stencil buffer */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  /* face culling */
  glDisable(GL_CULL_FACE);
}

void Engine::LoadShaders()
{
  ShadersManager::LoadShaderProgram(
    "TestingShader",
    ShadersManager::GetShaderFile("Testing.vert"),
    ShadersManager::GetShaderFile("Testing.frag"));
  ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFile("Instancing.vert"),
    ShadersManager::GetShaderFile("Scene.frag"));
  ShadersManager::LoadShaderProgram(
    "SceneShader",
    ShadersManager::GetShaderFile("Scene.vert"),
    ShadersManager::GetShaderFile("Scene.frag"));
  ShadersManager::LoadShaderProgram(
    "FramebufferShader",
    ShadersManager::GetShaderFile("Framebuffer.vert"),
    ShadersManager::GetShaderFile("Framebuffer.frag"));
}
