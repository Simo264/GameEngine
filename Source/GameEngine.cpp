#include "Core.hh"

#include "Window.hh"
#include "Camera.hh"
#include "Texture2D.hh"

#include "Graphics/VertexArray.hh"
#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"

#include "ResourceManager/ShadersManager.hh"
#include "ResourceManager/TexturesManager.hh"
#include "ResourceManager/FontsManager.hh"

#include "Mesh/Mesh.hh"
#include "Mesh/InstancedMesh.hh"
#include "Mesh/CubeMesh.hh"
#include "Mesh/InstancedCubeMesh.hh"

#include "Lighting/DirectionalLight.hh"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <spdlog/spdlog.h>

int main()
{
  // init libraries and create GLFW window
  // ---------------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4); // enable 4x MSAA on GLFW framebuffer
  Window window;
  window.Create(vec2u(720, 720), vec2u(650, 200), "OpenGL");
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1); // v-sync on
  //glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
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
  // ---------------------------------------


  // Init IMGui
  // ---------------------------------------
  FontsManager::Init();
  auto fontPath = FontsManager::GetFontByName("Karla-Regular.ttf");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 18); // custom font
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.Get(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
  // ---------------------------------------


  // Load shader programs
  // ---------------------------------------
  ShadersManager::Init();
  auto instancingShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFileByName("Instancing.vert"),
    ShadersManager::GetShaderFileByName("Default.frag")
  );
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);

  auto defaultShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFileByName("Default.vert"),
    ShadersManager::GetShaderFileByName("Default.frag")
  );
  defaultShader->Use();
  defaultShader->SetInt("Material.diffuse",  0); // sampler2d
  defaultShader->SetInt("Material.specular", 1); // sampler2d
  defaultShader->SetFloat("Material.shininess", 64.0f);
  // ---------------------------------------


  // Load textures from Textures directory
  // ---------------------------------------
  TexturesManager::Init();
  auto textureContainerDiff = TexturesManager::GetTextureByName("container_diffuse.png");
  auto textureContainerSpec = TexturesManager::GetTextureByName("container_specular.png");
  auto textureFloor   = TexturesManager::GetTextureByName("floor-grass.png");
  // ---------------------------------------


  // Mesh objects
  // ---------------------------------------
  CubeMesh cubeMesh;
  // ---------------------------------------


  // Lighting
  // ---------------------------------------
  Lighting::DirectionalLight dirLight("DirLight");
  // ---------------------------------------
  

  // Camera object
  // ---------------------------------------
  Camera camera;
  // ---------------------------------------


  // Loop
  // ---------------------------------------
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Graphics::Renderer::numRenderCallsPerFrame = 0;

    // Per-frame time logic
    // ---------------------------------------
    double now = glfwGetTime();
    double deltaTime = now - lastUpdateTime;
    // ---------------------------------------


    // input
    // ---------------------------------------
    glfwPollEvents();
    window.ProcessKeyboardInput();
    camera.ProcessInput(window, deltaTime);
    
    const mat4f projection = glm::perspective(glm::radians(camera.fov), 720.0f / 720.0f, 0.1f, 100.0f);
    const mat4f view = camera.GetViewMatrix();
    // ---------------------------------------


    // render
    // ---------------------------------------
    glClearColor(0.1f, 0.4f, 0.4f, 1.0f);               // values for the color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers to preset values

    // render cubes no instancing
    defaultShader->Use();
    defaultShader->SetMat4f("Projection", projection);
    defaultShader->SetMat4f("View",       view);
    defaultShader->SetVec3f("ViewPos",    camera.position);
    
    dirLight.Render(defaultShader);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureContainerDiff->textureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureContainerSpec->textureID);
    
    defaultShader->SetMat4f("Model", cubeMesh.Model());
    Graphics::Renderer::DrawArrays(cubeMesh.vertexArray);
    // ---------------------------------------
    auto time = glfwGetTime();
    dirLight.direction.x = glm::sin(time) * 5;
    


    double end = glfwGetTime();
    double renderTimeMs = (end - now) * 10e3;

    if (ImGui::Begin("Stats"))
    {
      ImGui::SetWindowSize({ 720, 120 });
      ImGui::SetWindowPos({ 0, 600 });

      string str;
      str.append("numRenderCallsPerFrame: " + std::to_string(Graphics::Renderer::numRenderCallsPerFrame));
      ImGui::Text(str.c_str());
      str.clear();
      
      str.append("RenderTime: " + std::to_string(renderTimeMs) + "ms");
      ImGui::Text(str.c_str());
      str.clear();
    } 
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // set lastUpdateTime every iteration
    lastUpdateTime = now;

    window.SwapBuffers();
  }

  // IMGui shutdown
  // ------------------------------
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();


  // GLFW: terminate, clearing all previously allocated GLFW resources.
  // -----------------------------------------------------------------
  window.Destroy();
  glfwTerminate();
	
  return 0;
}

