#include "Core.hh"

#include "Window.hh"
#include "Camera.hh"
#include "Texture2D.hh"

#include "Graphics/VertexArray.hh"
#include "Graphics/Shader.hh"
#include "Graphics/Renderer.hh"

#include "Pool/ShaderPool.hh"
#include "Pool/TexturePool.hh"

#include "Mesh/Cube.hh"

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

  auto shader = ShaderPool::LoadShader(
    "TestShader",
    ShaderPool::GetShaderFileByName("TestShader.vert"),
    ShaderPool::GetShaderFileByName("TestShader.frag")
  );
  shader->Use();
  shader->SetInt("ourTexture", 0);


  // Load textures from Textures directory
  // ---------------------------------------
  TexturePool::Init();
  auto textureContainer = TexturePool::GetTextureByName("container.jpg");


  // Mesh object
  // ---------------------------------------
  Cube cube;

  constexpr uint32_t numInstances = 10;
  mat4f models[numInstances];
  


  // Camera object
  // ---------------------------------------
  Camera camera;


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

  // Loop
  // ---------------------------------------
  const double fpsLimit = 1.0 / 60.0;
  double lastUpdateTime = 0;  // number of seconds since the last loop
  double lastFrameTime  = 0;  // number of seconds since the last frame
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
#ifdef LOCK_FPS
    if ((now - lastFrameTime) >= fpsLimit)
#endif
    {
      // draw scene here
      // ---------------------------------------
      glClearColor(0.1f, 0.4f, 0.4f, 1.0f);               // values for the color buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers to preset values

      shader->Use();
      shader->SetMat4f("projection", projection);
      shader->SetMat4f("view",       view);

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
    
      // only set lastFrameTime when you actually draw something
      lastFrameTime = now;

      window.SwapBuffers();
    }

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






