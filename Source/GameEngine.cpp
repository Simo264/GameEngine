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

  glfwSetInputMode(window.Get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


  // Load textures from Textures directory
  // ---------------------------------------
  TexturePool::Init();
  auto textureContainer = TexturePool::GetTextureByName("container.jpg");


  // Mesh object
  // ---------------------------------------
#if 0
  Cube cube;
  constexpr uint32_t numInstances = 10;
  mat4f models[numInstances];

  float rotations[numInstances] = {
    0.f,
    45.0f,
    90.0f,
    135.0f,
    180.0f,
    0.f,
    45.0f,
    90.0f,
    135.0f,
    180.0f
  };
#endif

  float vertices[] = {
   1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right front
   1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right front
  -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left front
  -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left front

   1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top right back
   1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // bottom right back
  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, // bottom left back
  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, // top left back
  };
  uint32_t indices[] = { 
    // front
    0,1,3,
    1,2,3,
    // back
    4,5,6,
    6,7,4,
    // right
    0,1,5,
    4,5,0,
    // left
    2,3,6,
    6,3,7,
    // top
    0,3,4,
    3,4,7,
    // bottom
    1,2,5,
    1,5,6
  };


  
  CubeMesh cubes[10];
  for (int i = 0; i < 10; i++)
    cubes[i].position.x = i * 3.0f;



  InstancedMesh instancedMeshes;
  Graphics::VAConfiguration config;
  config.PushAttribute(3);
  config.PushAttribute(2);
  instancedMeshes.Init(sizeof(vertices), vertices, sizeof(indices), indices, config);

  // Camera object
  // ---------------------------------------
  Camera camera;




  // Loop
  // ---------------------------------------
  double lastUpdateTime = 0;  // number of seconds since the last loop
  while (window.Loop())
  {
    Graphics::Renderer::numRenderCallsPerFrame = 0;

    // Per-frame time logic
    // ---------------------------------------
    double now = glfwGetTime();
    double deltaTime = now - lastUpdateTime;

    // input
    // ---------------------------------------
    glfwPollEvents();
    window.ProcessKeyboardInput();
    camera.ProcessInput(window, deltaTime);

    const mat4f projection = glm::perspective(glm::radians(camera.fov), 720.0f/720.0f, 0.1f, 100.0f);
    const mat4f view       = camera.GetViewMatrix();

    // render
    // ---------------------------------------
    glClearColor(0.1f, 0.4f, 0.4f, 1.0f);               // values for the color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers to preset values

    defaultShader->Use();
    defaultShader->SetMat4f("projection", projection);
    defaultShader->SetMat4f("view",       view);

    // render cubes no instancing
    for (int i = 0; i < 10; i++)
    {
      defaultShader->SetMat4f("model", cubes[i].Model());
      Graphics::Renderer::DrawIndexed(cubes[i].vertexArray);
    }
    
    instancingShader->Use();
    instancingShader->SetMat4f("projection", projection);
    instancingShader->SetMat4f("view", view);

    // render cubes instancing
    Graphics::Renderer::DrawIndexedInstanced(instancedMeshes.vertexArray, instancedMeshes.MAX_NUM_INSTANCES);

#if 0
    for (int i = 0; i < numInstances; i++)
    {
      models[i] = glm::translate(mat4f(1.0f), vec3f(i * 3.f, 0.0f, 0.0f)) * 
                  glm::rotate(mat4f(1.0f), rotations[i], vec3f(0.0f, 0.0f, 1.0f));
        
      rotations[i] += 0.05f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, cube.instanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(models), models);
      
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, textureContainer->textureID);

    Graphics::Renderer::DrawArraysInstanced(cube.vertexArray, numInstances);
#endif

    window.SwapBuffers();

    // set lastUpdateTime every iteration
    lastUpdateTime = now;

    spdlog::info("numRenderCallsPerFrame={}", Graphics::Renderer::numRenderCallsPerFrame);
  }


  // GLFW: terminate, clearing all previously allocated GLFW resources.
  // ---------------------------------------
  window.Destroy();
  glfwTerminate();
	
  return 0;
}






