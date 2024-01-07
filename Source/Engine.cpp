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
  
  // Load textures from Textures directory
  // ---------------------------------------
  TexturesManager::Initialize();
}

void Engine::Run()
{
  auto testingShader      = ShadersManager::GetShader("TestingShader");
  auto instancingShader   = ShadersManager::GetShader("InstancingShader");
  auto sceneShader        = ShadersManager::GetShader("SceneShader");
  auto framebufferShader  = ShadersManager::GetShader("FramebufferShader");

  Window window(glfwGetCurrentContext());

  // Camera object
  // ---------------------------------------
  Camera camera(window.GetWindowSize(), Vec3f(0.0f, 0.0f, 10.0f));
  // ---------------------------------------

  // Mesh objects
  // ---------------------------------------
  Plane plane1;
  Plane plane2;
  Plane plane3;
  Plane plane4;
  plane1.position = Vec3f(0.0f, 0.0f, 0.0f);
  plane2.position = Vec3f(2.0f, 0.0f, 0.0f);
  plane3.position = Vec3f(0.0f, 0.0f, 2.0f);
  plane4.position = Vec3f(2.0f, 0.0f, 2.0f);


  // ---------------------------------------

  // Lighting
  // ---------------------------------------
  DirectionalLight dirLight("DirLight");
  PointLight pointLight("PointLight");
  // ---------------------------------------
  
  // Create scene
  // ---------------------------------------
  Scene scene;
  scene.directionalLight = &dirLight;
  scene.AddPointLight(&pointLight);
  scene.AddStaticMesh(&plane1);
  scene.AddStaticMesh(&plane2);
  scene.AddStaticMesh(&plane3);
  scene.AddStaticMesh(&plane4);
  // ---------------------------------------

  // Framebuffer object
  // ---------------------------------------
  //FrameBuffer framebuffer;
  //framebuffer.Create(window.GetFramebufferSize());
  // ---------------------------------------
  

  // Loop
  // ---------------------------------------
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    editor.NewFrame();

    // Per-frame time logic
    // ---------------------------------------
    const double now = glfwGetTime();
    const double deltaTime = now - lastUpdateTime;
    const Vec2i windowFbSize = window.GetFramebufferSize();
    const float aspectRatio = (float)(windowFbSize.x / windowFbSize.y);
    Renderer::drawCalls = 0;
    // ---------------------------------------


    // input
    // ---------------------------------------
    glfwPollEvents();
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
      window.CloseWindow();
    camera.ProcessInput(&window, deltaTime);
    const Mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    const Mat4f view = camera.GetViewMatrix();
    sceneShader->Use();
    sceneShader->SetMat4f("Projection", projection);
    sceneShader->SetMat4f("View", view);

    // Render scene
    // ---------------------------------------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // make sure we clear the framebuffer's content
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.DrawScene(sceneShader);

    // Render editor
    // ---------------------------------------
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
  // Shutdown Editor
  // -----------------------------------------------------------------
  editor.ShutDown();


  // GLFW: terminate, clearing all previously allocated GLFW resources.
  // -----------------------------------------------------------------
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
  
  // FramebufferSizeCallback
  glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
  });
  // WindowSizeCallback
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
  });
  
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // antialising
  glEnable(GL_MULTISAMPLE);
  
  // depth buffer
  glEnable(GL_DEPTH_TEST);
  
  // blending/stencil buffer
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // face culling
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Engine::LoadShaders()
{
  auto testingShader = ShadersManager::LoadShaderProgram(
    "TestingShader",
    ShadersManager::GetShaderFile("Testing.vert"),
    ShadersManager::GetShaderFile("Testing.frag"));

  auto instancingShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFile("Instancing.vert"),
    ShadersManager::GetShaderFile("Scene.frag"));
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);

  auto sceneShader = ShadersManager::LoadShaderProgram(
    "SceneShader",
    ShadersManager::GetShaderFile("Scene.vert"),
    ShadersManager::GetShaderFile("Scene.frag"));
  sceneShader->Use();
  sceneShader->SetInt("Material.diffuse", 0); // sampler2d
  sceneShader->SetInt("Material.specular", 1); // sampler2d
  sceneShader->SetFloat("Material.shininess", 64.0f);

  auto framebufferShader = ShadersManager::LoadShaderProgram(
    "FramebufferShader",
    ShadersManager::GetShaderFile("Framebuffer.vert"),
    ShadersManager::GetShaderFile("Framebuffer.frag"));
  framebufferShader->Use();
  framebufferShader->SetInt("screenTexture", 0);
}
