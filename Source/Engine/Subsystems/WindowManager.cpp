#include "WindowManager.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/OpenGL.hpp"
#include <GLFW/glfw3.h>

void WindowManager::Initialize()
{
  int success = glfwInit();
  if (success != GLFW_TRUE)
  {
    CONSOLE_CRITICAL("Error on initializing GLFW");
    exit(EXIT_FAILURE);
  }
  CONSOLE_INFO("GLFW initialized");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  /* Set default window values */
  _context = glfwCreateWindow(1600, 900, "GameEngine", nullptr, nullptr); /* 1600x900 */
  if (!_context)
  {
    CONSOLE_CRITICAL("Error on creating window");
    exit(EXIT_FAILURE);
  }
  CONSOLE_INFO("Window created");

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
    CONSOLE_CRITICAL("Error on loading OpenGL");
    exit(EXIT_FAILURE);
  }
  CONSOLE_INFO("OpenGL loaded");
}

void WindowManager::CleanUp() const
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

vec2i32 WindowManager::GetWindowSize() const
{
  vec2i32 res{};
  glfwGetWindowSize(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetWindowSize(int w, int h) const
{
  glfwSetWindowSize(_context, w, h);
}

vec2i32 WindowManager::GetFramebufferSize() const
{
  vec2i32 res{};
  glfwGetFramebufferSize(_context, &res.x, &res.y);
  return res;
}

int WindowManager::GetKey(uint32_t key) const 
{ 
  return glfwGetKey(_context, key); 
}

int WindowManager::GetMouseKey(uint32_t key) const 
{ 
  return glfwGetMouseButton(_context, key); 
}

vec2d WindowManager::GetCursorPosition() const
{
  vec2d res{};
  glfwGetCursorPos(_context, &res.x, &res.y);
  return res;
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
  glfwSwapInterval(b);
}

void WindowManager::MakeContextCurrent(Context context)
{
  glfwMakeContextCurrent(context);
  _context = context;
}
