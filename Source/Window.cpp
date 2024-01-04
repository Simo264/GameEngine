#include "Window.hh"

Vec2i Window::GetWindowSize()
{
  Vec2i  size;
  glfwGetWindowSize(_window, &size.x, &size.y);
  return size;
}

Vec2i  Window::GetFramebufferSize()
{
  Vec2i  size;
  glfwGetFramebufferSize(_window, &size.x, &size.y);
  return size;
}
