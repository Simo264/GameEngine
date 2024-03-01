#include "WindowManager.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include <GLFW/glfw3.h>

void WindowManager::Initialize()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  /* Load OpenGL 4.6 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  /* Set default window values */
  _context = glfwCreateWindow(1600, 900, "GameEngine", nullptr, nullptr); /* 1600x900 */
  glfwMakeContextCurrent(_context);
  glfwSetWindowPos(_context, 50, 50);   
  glfwSetWindowAspectRatio(_context, 16, 9);   /* 16:9 */
  glfwSwapInterval(0);  /* No vsync */

  glfwSetWindowSizeCallback(_context, [](GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
  });

  /* Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error. */
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    CONSOLE_ERROR("Failed to initialize OpenGL context");
    exit(-1);
  }

  /* Antialising OFF */
  glEnable(GL_MULTISAMPLE);

  /* Depth buffer ON */
  glEnable(GL_DEPTH_TEST);

  /* Blending OFF */
  glDisable(GL_BLEND);

  /* Face culling OFF */
  glDisable(GL_CULL_FACE);

  /* Gamma correction ON */
  glDisable(GL_FRAMEBUFFER_SRGB);
}

void WindowManager::CleanUp()
{
  glfwDestroyWindow(_context);
  glfwTerminate();
}

void WindowManager::PoolEvents() const
{
  glfwPollEvents();
}

void WindowManager::Close() const
{ 
  glfwSetWindowShouldClose(_context, GLFW_TRUE);
}

bool WindowManager::IsOpen() const 
{ 
  return !glfwWindowShouldClose(_context); 
}

WindowManager::Context WindowManager::GetContext() const
{ 
  return _context;
}

void WindowManager::SetContext(WindowManager::Context newcontext)
{
  _context = newcontext;
}

void WindowManager::SetWindowTitle(const char* title) const
{
  glfwSetWindowTitle(_context, title);
}

void WindowManager::SetWindowAspectRatio(int numer, int denom) const
{
  glfwSetWindowAspectRatio(_context, numer, denom);
}

void WindowManager::SwapWindowBuffers() const 
{ 
  glfwSwapBuffers(_context); 
}

void WindowManager::GetWindowSize(int& w, int& h) const
{
  glfwGetWindowSize(_context, &w, &h);
}

void WindowManager::SetWindowSize(int w, int h) const
{
  glfwSetWindowSize(_context, w, h);
}

void WindowManager::GetFramebufferSize(int& w, int& h) const
{
  glfwGetFramebufferSize(_context, &w, &h);
}

int WindowManager::GetKey(uint32_t key) const 
{ 
  return glfwGetKey(_context, key); 
}

int WindowManager::GetMouseKey(uint32_t key) const 
{ 
  return glfwGetMouseButton(_context, key); 
}

void WindowManager::GetCursorPosition(double& x, double& y) const
{ 
  glfwGetCursorPos(_context, &x, &y); 
}

void WindowManager::SetCursorMode(int value) const 
{ 
  glfwSetInputMode(_context, GLFW_CURSOR, value); 
}

void WindowManager::SetWindowPosition(int x, int y) const
{ 
  glfwSetWindowPos(_context, x, y); 
}

void WindowManager::SetWindowVsync(bool b) const
{
  glfwSwapInterval((int)b);
}
