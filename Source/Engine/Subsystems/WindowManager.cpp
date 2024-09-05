#include "WindowManager.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include <GLFW/glfw3.h>

void WindowManager::Initialize(WindowProps props)
{
  i32 success = glfwInit();
  assert(success == GLFW_TRUE);
  CONSOLE_INFO("GLFW initialized");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  _context = glfwCreateWindow(props.size.x, props.size.y, props.title.data(), nullptr, nullptr);
  assert(_context);
  CONSOLE_INFO("Window created");

  glfwMakeContextCurrent(_context);
  glfwSetWindowPos(_context, props.position.x, props.position.y);
  glfwSetWindowAspectRatio(_context, props.aspectRatio.x, props.aspectRatio.y);
  glfwSwapInterval(static_cast<int>(props.vsync));

  glfwSetWindowSizeCallback(_context, [](GLFWwindow* window, i32 width, i32 height) {
    glfwSetWindowSize(window, width, height);
  });

  /* Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error. */
  i32 version = gladLoadGL(glfwGetProcAddress);
  assert(version != 0);
  CONSOLE_INFO("OpenGL loaded");
}

void WindowManager::CleanUp() const
{
  //glfwDestroyWindow(_context);
  //glfwTerminate();
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

void WindowManager::SetWindowAspectRatio(i32 numer, i32 denom) const
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

void WindowManager::SetWindowSize(i32 w, i32 h) const
{
  glfwSetWindowSize(_context, w, h);
}

vec2i32 WindowManager::GetFramebufferSize() const
{
  vec2i32 res{};
  glfwGetFramebufferSize(_context, &res.x, &res.y);
  return res;
}

i32 WindowManager::GetKey(u32 key) const 
{ 
  return glfwGetKey(_context, key); 
}

i32 WindowManager::GetMouseKey(u32 key) const 
{ 
  return glfwGetMouseButton(_context, key); 
}

vec2d WindowManager::GetCursorPosition() const
{
  vec2d res{};
  glfwGetCursorPos(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetCursorMode(i32 value) const 
{ 
  glfwSetInputMode(_context, GLFW_CURSOR, value); 
}

void WindowManager::SetWindowPosition(i32 x, i32 y) const
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

const char* WindowManager::GetVersion() const
{
  return glfwGetVersionString();
}

vec2i32 WindowManager::GetWindowPos() const
{
  vec2i32 res{};
  glfwGetWindowPos(_context, &res.x, &res.y);
  return res;
}
