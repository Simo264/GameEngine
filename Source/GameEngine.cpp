#include "Core.hh"

#include "Window.hh"
#include "Camera.hh"
#include "Texture2D.hh"

#include "Graphics/VertexArray.hh"
#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"

#include "Pool/ShaderPool.hh"
#include "Pool/TexturePool.hh"

#include "Mesh/Mesh.hh"
#include "Mesh/InstancedMesh.hh"
#include "Mesh/CubeMesh.hh"
#include "Mesh/InstancedCubeMesh.hh"

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

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
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.Get(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
  // ---------------------------------------


  // Load shader programs
  // ---------------------------------------
  ShaderPool::Init();
  auto instancingShader = ShaderPool::LoadShader(
    "InstancingShader",
    ShaderPool::GetShaderFileByName("Instancing.vert"),
    ShaderPool::GetShaderFileByName("Default.frag")
  );
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);

  auto defaultShader = ShaderPool::LoadShader(
    "InstancingShader",
    ShaderPool::GetShaderFileByName("Default.vert"),
    ShaderPool::GetShaderFileByName("Default.frag")
  );
  defaultShader->Use();
  defaultShader->SetInt("ourTexture", 0);
  // ---------------------------------------


  // Load textures from Textures directory
  // ---------------------------------------
  TexturePool::Init();
  auto textureContainer = TexturePool::GetTextureByName("container.jpg");
  // ---------------------------------------


  // Mesh objects
  // ---------------------------------------
  CubeMesh cubes[10];
  for (int i = 0; i < 10; i++)
  {
    cubes[i].position.x = i * 3.0f;
    cubes[i].position.y = 3.0f;
  }

  InstancedCubeMesh instancedCubes(10);

  mat4f models[10];
  for (int i = 0; i < 10; i++)
    models[i] = glm::translate(mat4f(1.0f), vec3f(i * 3.0f, 0.0, 0.0f));

  instancedCubes.SetInstancesModelRange(0, 10, models);
  
  models[9] = glm::translate(mat4f(1.0f), vec3f(30.0f, 0.0, 0.0f)) * 
              glm::rotate(mat4f(1.0f), glm::radians(45.0f), vec3f(0.0f, 0.0f, 1.0f));
  
  instancedCubes.SetInstanceModel(&models[9], 10);
  
  instancedCubes.SetInstanceNumber(10);
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
    double start = glfwGetTime();
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
    defaultShader->SetMat4f("projection", projection);
    defaultShader->SetMat4f("view",       view);
    for (int i = 0; i < 10; i++)
    {
      defaultShader->SetMat4f("model", cubes[i].Model());
      Graphics::Renderer::DrawIndexed(cubes[i].vertexArray);
    }
    
    // render cubes instancing
    instancingShader->Use();
    instancingShader->SetMat4f("projection", projection);
    instancingShader->SetMat4f("view", view);
    Graphics::Renderer::DrawIndexedInstanced(instancedCubes.vertexArray, instancedCubes.GetInstanceNumber());
    // ---------------------------------------


    double end = glfwGetTime();
    double renderTimeMs = (end - start) * 10e3;

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






