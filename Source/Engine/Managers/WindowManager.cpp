#include "WindowManager.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Globals.hpp"

#include <GLFW/glfw3.h>

void WindowManager::Initialize(WindowProps props)
{
  auto result = glfwInit();
  assert(result == GLFW_TRUE && "Failed to initialize GLFW library");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, props.contextVersionMinor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, props.contextVersionMajor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  glfwWindowHint(GLFW_SAMPLES, props.samples); // Enable 4x MSAA on GLFW frame buffer
  glfwWindowHint(GLFW_DEPTH_BITS, 24);

  _context = glfwCreateWindow(props.size.x, props.size.y, props.title.data(), nullptr, nullptr);
  assert(_context && "Failed to create window");

  glfwMakeContextCurrent(_context);
  glfwSetWindowPos(_context, props.position.x, props.position.y);
  glfwSetWindowAspectRatio(_context, props.aspectRatio.x, props.aspectRatio.y);
  glfwSwapInterval(static_cast<i32>(props.vsync));

  glfwSetWindowSizeCallback(_context, [](GLFWwindow *window, i32 width, i32 height)
                            { glfwSetWindowSize(window, width, height); });

  // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
  result = gladLoadGL(glfwGetProcAddress);
  assert(result != 0 && "Failed to load OpenGL APIs");
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

void WindowManager::SetWindowTitle(StringView title) const
{
  glfwSetWindowTitle(_context, title.data());
}

void WindowManager::SetWindowAspectRatio(i32 numer, i32 denom) const
{
  glfwSetWindowAspectRatio(_context, numer, denom);
}

void WindowManager::SwapWindowBuffers() const
{
  glfwSwapBuffers(_context);
}

Vec2I WindowManager::GetWindowSize() const
{
  auto res = Vec2I{};
  glfwGetWindowSize(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetWindowSize(i32 w, i32 h) const
{
  glfwSetWindowSize(_context, w, h);
}

Vec2I WindowManager::GetFramebufferSize() const
{
  auto res = Vec2I{};
  glfwGetFramebufferSize(_context, &res.x, &res.y);
  return res;
}

InputAction WindowManager::GetKey(InputKey key) const
{
  return static_cast<InputAction>(glfwGetKey(_context, static_cast<i32>(key)));
}

InputAction WindowManager::GetMouseKey(InputMouseKey key) const
{
  return static_cast<InputAction>(glfwGetMouseButton(_context, static_cast<i32>(key)));
}

Vec2D WindowManager::GetCursorPosition() const
{
  auto res = Vec2D{};
  glfwGetCursorPos(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetCursorMode(CursorMode mode) const
{
  glfwSetInputMode(_context, GLFW_CURSOR, static_cast<i32>(mode));
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

const char *WindowManager::GetVersion() const
{
  return glfwGetVersionString();
}

Vec2I WindowManager::GetWindowPos() const
{
  auto res = Vec2I{};
  glfwGetWindowPos(_context, &res.x, &res.y);
  return res;
}

f64 WindowManager::GetWorldTime() const
{
  return glfwGetTime();
}
