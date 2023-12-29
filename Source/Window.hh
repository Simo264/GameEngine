#pragma once

#include "Core.hh"

// Wrapper class for GLFWwindow
// ----------------------------------
class Window
{
public:

  Window(GLFWwindow* window) : _window{ window } { };
  ~Window() = default;

  // glfw: set close flag TRUE
  void CloseWindow() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }

  // glfw: render loop
  bool Loop() const { return !glfwWindowShouldClose(_window); }

  // return GLFWwindow* object
  GLFWwindow* GetContext() const { return _window; }

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  void SwapWindowBuffers() { glfwSwapBuffers(_window); }

  // glfw: get window size
  vec2i GetWindowSize();
  
  // glfw: retrieves the size, in pixels, of the framebuffer of the specified window
  vec2i GetFramebufferSize();

  // glfw: keyboard key
  int GetKey(uint32_t key) const { return glfwGetKey(_window, key); }

  // glfw: mouse Key
  int GetMouseKey(uint32_t key) const { return glfwGetMouseButton(_window, key); }

  // glfw: mouse position 
  void GetCursorPosition(vec2d& pos) { glfwGetCursorPos(_window, &pos.x, &pos.y); }

  // glfw: value = GLFW_CURSOR_NORMAL | GLFW_CURSOR_HIDDEN | GLFW_CURSOR_DISABLED
  void SetCursorMode(int value) { glfwSetInputMode(_window, GLFW_CURSOR, value); }

  // glfw: window position
  void SetPosition(vec2i pos)  { glfwSetWindowPos(_window, pos.x, pos.y); }

private:
  GLFWwindow* _window;
};
