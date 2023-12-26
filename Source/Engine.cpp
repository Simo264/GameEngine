#include "Engine.hh"
#include "Logger.hh"
#include "Window.hh"
#include "Camera.hh"
#include "Model.hh"

#include "Mesh/CubeMesh.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

#include "ResourceManager/ShadersManager.hh"
#include "ResourceManager/TexturesManager.hh"
#include "ResourceManager/FontsManager.hh"

#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"
#include "Graphics/FrameBuffer.hh"

#include "Imgui/imgui.h" 
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

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

  // Init IMGui
  // ---------------------------------------
  InitImGui();

  // Load shaders 
  // ---------------------------------------
  ShadersManager::Initialize();
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
  Camera camera(window.GetWindowSize());
  // ---------------------------------------

  // Mesh objects
  // ---------------------------------------
  Model cubeModel("Shapes/Cube/Cube.obj");
  cubeModel.scaling = vec3f(0.5f, 0.5f, 0.5f);
  cubeModel.position.y = -0.49f;

  Model planeModel("Shapes/Plane/Plane.obj");
  planeModel.position.y = -1.0f;
  planeModel.scaling = vec3f(10.0f, 0.0f, 10.0f);
  // ---------------------------------------

  // Lighting
  // ---------------------------------------
  DirectionalLight dirLight("DirLight");
  //dirLight.ambient = 0.07f;
  //dirLight.diffuse = 0.0f;
  //dirLight.specular = 0.0f;

  //PointLight pointLight("PointLight");
  //pointLight.position = vec3f(0.0f, 5.0f, 0.0f);
  // ---------------------------------------
  

  // Framebuffer object
  // ---------------------------------------
  FrameBuffer framebuffer;
  framebuffer.Create(window.GetFramebufferSize());
  // ---------------------------------------
  

  // Loop
  // ---------------------------------------
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    //Start the Dear ImGui frame
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();

    // Per-frame time logic
    // ---------------------------------------
    double now = glfwGetTime();
    double deltaTime = now - lastUpdateTime;
    const vec2i windowFbSize = window.GetFramebufferSize();
    const float aspectRatio = (float)(windowFbSize.x / windowFbSize.y);
    Renderer::drawCalls = 0;
    // ---------------------------------------


    // input
    // ---------------------------------------
    glfwPollEvents();
    if (window.GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
      window.CloseWindow();

    camera.ProcessInput(window, deltaTime);
    const mat4f projection = glm::perspective(glm::radians(camera.fov), aspectRatio, 0.1f, 100.0f);
    const mat4f view = camera.GetViewMatrix();



    // render
    // ---------------------------------------
    framebuffer.RescaleFrameBuffer(windowFbSize);
    framebuffer.Bind();      // bind to framebuffer and draw scene as we normally would to color texture 
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // make sure we clear the framebuffer's content
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneShader->Use();
    sceneShader->SetMat4f("Projection", projection);
    sceneShader->SetMat4f("View", view);
    
    dirLight.Render(sceneShader);
    cubeModel.Draw(sceneShader);
    planeModel.Draw(sceneShader);
    
    framebuffer.Unbind();                 // now bind back to default framebuffer 
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // clear all relevant buffers
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    framebufferShader->Use();
    framebuffer.DrawFrame(framebufferShader);

    lastUpdateTime = now;
    
    window.SwapWindowBuffers();
  }
}

void Engine::ShutDown()
{
  // IMGui shutdown
  // -----------------------------------------------------------------
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();


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
  glfwSwapInterval(1);                      // v-sync on
  // glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
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

void Engine::InitImGui()
{
  auto fontPath = FontsManager::GetFont("Karla-Regular.ttf");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); // custom font
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
}
