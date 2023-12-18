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

#include "Model.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
  auto fontPath = FontsManager::GetFont("Karla-Regular.ttf");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
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
    ShadersManager::GetShaderFile("Instancing.vert"),
    ShadersManager::GetShaderFile("Default.frag")
  );
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);

  auto defaultShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFile("Default.vert"),
    ShadersManager::GetShaderFile("Default.frag")
  );
  defaultShader->Use();
  defaultShader->SetInt("Material.diffuse",  0); // sampler2d
  defaultShader->SetInt("Material.specular", 1); // sampler2d
  defaultShader->SetFloat("Material.shininess", 64.0f);
  // ---------------------------------------


  // Load textures from Textures directory
  // ---------------------------------------
  TexturesManager::Init();
  auto textureContainerDiff = TexturesManager::GetTexture("container_diffuse.png");
  auto textureContainerSpec = TexturesManager::GetTexture("container_specular.png");
  auto textureFloor = TexturesManager::GetTexture("floor-grass.png");
  // ---------------------------------------


  // Mesh objects
  // ---------------------------------------
  const auto assetsDirPath = (std::filesystem::current_path().parent_path()) / "Assets";
  
  auto crateObjPath = assetsDirPath / "Crate" / "Crate.obj";
  Model modelCrate(crateObjPath);
  modelCrate.scaling  = vec3f(0.5f, 0.5f, 0.5f);

  CubeMesh cubeMesh;
  cubeMesh.textureDiffuse = textureContainerDiff;
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
    modelCrate.Draw(defaultShader);

    mat4f cubeMeshModel = glm::translate(mat4f(1.0f), vec3f(0.0f, 5.0f, 0.0f));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeMesh.textureDiffuse->textureID);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, textureContainerSpec->textureID);
    defaultShader->SetMat4f("Model", cubeMeshModel);
    Graphics::Renderer::DrawArrays(cubeMesh.vertexArray);
    // ---------------------------------------
    
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

