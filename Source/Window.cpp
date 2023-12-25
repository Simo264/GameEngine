#include "Window.hh"

vec2i Window::GetWindowSize()
{
  vec2i size;
  glfwGetWindowSize(_window, &size.x, &size.y);
  return size;
}

vec2i Window::GetFramebufferSize()
{
  vec2i size;
  glfwGetFramebufferSize(_window, &size.x, &size.y);
  return size;
}
