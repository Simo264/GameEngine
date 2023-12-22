#include "Core.hh"

#include "Window.hh"
#include "Camera.hh"
#include "Texture2D.hh"
#include "Model.hh"

#include "Mesh/CubeMesh.hh"

#include "Graphics/VertexArray.hh"
#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"
#include "Graphics/FrameBuffer.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

#include "ResourceManager/ShadersManager.hh"
#include "ResourceManager/TexturesManager.hh"
#include "ResourceManager/FontsManager.hh"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_spectrum.h>

#include <spdlog/spdlog.h>

uint32_t WINDOW_WIDTH  = 720;
uint32_t WINDOW_HEIGHT = 720;

void SetupImGuiStyle();

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
  window.Create(vec2u(WINDOW_WIDTH, WINDOW_HEIGHT), vec2u(300, 50), "OpenGL");
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1); // v-sync on
  // glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
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
  io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16); // custom font
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  SetupImGuiStyle();
  ImGui_ImplGlfw_InitForOpenGL(window.Get(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
  float alpha = 0.50f;
  // ---------------------------------------


  // Load shader programs
  // ---------------------------------------
  ShadersManager::Init();
  auto testingShader = ShadersManager::LoadShaderProgram(
    "TeestingShader",
    ShadersManager::GetShaderFile("Testing.vert"),
    ShadersManager::GetShaderFile("Testing.frag")
  );
  auto instancingShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFile("Instancing.vert"),
    ShadersManager::GetShaderFile("Default.frag")
  );
  instancingShader->Use();
  instancingShader->SetInt("ourTexture", 0);
  auto sceneShader = ShadersManager::LoadShaderProgram(
    "InstancingShader",
    ShadersManager::GetShaderFile("Default.vert"),
    ShadersManager::GetShaderFile("Default.frag")
  );
  sceneShader->Use();
  sceneShader->SetInt("Material.diffuse",  0); // sampler2d
  sceneShader->SetInt("Material.specular", 1); // sampler2d
  sceneShader->SetFloat("Material.shininess", 64.0f);
  auto framebufferShader = ShadersManager::LoadShaderProgram(
    "FramebufferShader",
    ShadersManager::GetShaderFile("Framebuffer.vert"),
    ShadersManager::GetShaderFile("Framebuffer.frag")
  );
  framebufferShader->Use();
  framebufferShader->SetInt("screenTexture", 0);
  // ---------------------------------------


  // Load textures from Textures directory
  // ---------------------------------------
  TexturesManager::Init();
  auto textDefaultDiffuse = TexturesManager::GetTexture("default_diffuse.png");
  // ---------------------------------------


  // Mesh objects
  // ---------------------------------------
  //Model cubeModel("Shapes/Cube/Cube.obj");
  //cubeModel.scaling = vec3f(0.5f, 0.5f, 0.5f);
  //cubeModel.position.y = -0.49f;
  //
  //Model planeModel("Shapes/Plane/Plane.obj");
  //planeModel.position.y = -1.0f;
  //planeModel.scaling = vec3f(10.0f, 0.0f, 10.0f);

  CubeMesh cube;
  // ---------------------------------------


  // Lighting
  // ---------------------------------------
  //Lighting::DirectionalLight dirLight("DirLight");
  //dirLight.ambient = 0.07f;
  //dirLight.diffuse = 0.0f;
  //dirLight.specular = 0.0f;

  //Lighting::PointLight pointLight("PointLight");
  //pointLight.position = vec3f(0.0f, 5.0f, 0.0f);
  // ---------------------------------------
  

  // Camera object
  // ---------------------------------------
  Camera camera;
  // ---------------------------------------

  float screenVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  Graphics::VAConfiguration config;
  config.PushAttribute(2); // positions
  config.PushAttribute(2); // texCoords

  MeshData data{ sizeof(screenVertices), screenVertices , 0, nullptr };
  
  Mesh screenQuad;
  screenQuad.Init(data, config);

  
  // framebuffer configuration
  // -------------------------
  Graphics::FrameBuffer framebuffer(vec2f(WINDOW_WIDTH, WINDOW_HEIGHT));



  // Loop
  // ---------------------------------------
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Graphics::Renderer::drawCalls = 0;

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
    const mat4f projection = glm::perspective(glm::radians(camera.fov), (float)(WINDOW_WIDTH/WINDOW_HEIGHT) , 0.1f, 100.0f);
    const mat4f view = camera.GetViewMatrix();
    
    vec2i framebufferSize;
    window.GetFramebufferSize(framebufferSize);
    glViewport(0, 0, framebufferSize.x, framebufferSize.y);
    framebuffer.RescaleFrameBuffer(framebufferSize);
    // ---------------------------------------


    // render
    // ---------------------------------------
    // bind to framebuffer and draw scene as we normally would to color texture 
    framebuffer.Bind();
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
    
    // make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    testingShader->Use();
    testingShader->SetMat4f("Projection", projection);
    testingShader->SetMat4f("View",       view);
    testingShader->SetMat4f("Model",      glm::scale(mat4f(1.0f), vec3f(0.25f, 0.25f, 0.25f)));
    Graphics::Renderer::DrawArrays(cube.vertexArray);
    
    // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
    framebuffer.Unbind();

    // clear all relevant buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);


    ImGui::Begin("Scene");
    {
      //float width = ImGui::GetContentRegionAvail().x;
      //float height = ImGui::GetContentRegionAvail().y;
      ImGui::Image(
        (ImTextureID) framebuffer.GetFramebufferTexture(), 
        ImGui::GetContentRegionAvail(), 
        ImVec2(0, 1), 
        ImVec2(1, 0));
    }
    ImGui::End();
    // ---------------------------------------
    
    double end = glfwGetTime();
    double renderTimeMs = (end - now) * 10e3;
    
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


/* -----------------------------------------------------
 *          FUNCTIONS
 * -----------------------------------------------------
*/

void SetupImGuiStyle() {
  using namespace ImGui;

  ImGuiStyle* style = &ImGui::GetStyle();
  style->GrabRounding = 4.0f;

  ImVec4* colors = style->Colors;
  colors[ImGuiCol_Text] = ColorConvertU32ToFloat4(Spectrum::Static::BLACK); // text on hovered controls is gray900
  colors[ImGuiCol_TextDisabled] = ColorConvertU32ToFloat4(Spectrum::GRAY500);
  colors[ImGuiCol_WindowBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ColorConvertU32ToFloat4(Spectrum::GRAY50); // not sure about this. Note: applies to tooltips too.
  colors[ImGuiCol_Border] = ColorConvertU32ToFloat4(Spectrum::GRAY300);
  colors[ImGuiCol_BorderShadow] = ColorConvertU32ToFloat4(Spectrum::Static::NONE); // We don't want shadows. Ever.
  colors[ImGuiCol_FrameBg] = ColorConvertU32ToFloat4(Spectrum::GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
  colors[ImGuiCol_FrameBgHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY50);
  colors[ImGuiCol_FrameBgActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
  colors[ImGuiCol_TitleBg] = ColorConvertU32ToFloat4(Spectrum::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
  colors[ImGuiCol_TitleBgActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
  colors[ImGuiCol_TitleBgCollapsed] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
  colors[ImGuiCol_MenuBarBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100);
  colors[ImGuiCol_ScrollbarBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100); // same as regular background
  colors[ImGuiCol_ScrollbarGrab] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
  colors[ImGuiCol_ScrollbarGrabHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
  colors[ImGuiCol_ScrollbarGrabActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
  colors[ImGuiCol_CheckMark] = ColorConvertU32ToFloat4(Spectrum::BLUE500);
  colors[ImGuiCol_SliderGrab] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
  colors[ImGuiCol_SliderGrabActive] = ColorConvertU32ToFloat4(Spectrum::GRAY800);
  colors[ImGuiCol_Button] = ColorConvertU32ToFloat4(Spectrum::GRAY75); // match default button to Spectrum's 'Action Button'.
  colors[ImGuiCol_ButtonHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY50);
  colors[ImGuiCol_ButtonActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
  colors[ImGuiCol_Header] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
  colors[ImGuiCol_HeaderHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE500);
  colors[ImGuiCol_HeaderActive] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
  colors[ImGuiCol_Separator] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
  colors[ImGuiCol_SeparatorHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
  colors[ImGuiCol_SeparatorActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
  colors[ImGuiCol_ResizeGrip] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
  colors[ImGuiCol_ResizeGripHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
  colors[ImGuiCol_ResizeGripActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
  colors[ImGuiCol_PlotLines] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
  colors[ImGuiCol_PlotLinesHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
  colors[ImGuiCol_PlotHistogram] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
  colors[ImGuiCol_PlotHistogramHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
  colors[ImGuiCol_TextSelectedBg] = ColorConvertU32ToFloat4((Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight] = ColorConvertU32ToFloat4((Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}
