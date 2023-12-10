#include "Window.hh"

void Window::Create(vec2u dim, vec2u pos, const char* title)
{
  _width  = dim.x;
  _height = dim.y;
  _window = glfwCreateWindow(_width, _height, title, NULL, NULL);
  glfwMakeContextCurrent(_window);
  SetPosition(pos);
}

void Window::Destroy()
{
  if (_window)
    glfwDestroyWindow(_window);
}

void Window::ProcessKeyboardInput()
{
  if (GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
    Close();
}