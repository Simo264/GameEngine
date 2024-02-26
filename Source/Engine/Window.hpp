#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/* ----------------------------------------
      Wrapper class for GLFWwindow 
  ---------------------------------------- */
class Window
{
public:
  Window(GLFWwindow* window) : _window{ window } { };

  /* glfw: set close flag TRUE */
  void CloseWindow() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }

  /* glfw: render loop */
  bool Loop() const { return !glfwWindowShouldClose(_window); }

  /* return GLFWwindow * object */
  GLFWwindow* GetContext() const { return _window; }

  /* glfw: swap buffers and poll IO events(keys pressed / released, mouse moved etc.) */
  void SwapWindowBuffers() { glfwSwapBuffers(_window); }

  /* glfw: get window size */
  Vec2i GetWindowSize() const
  {
    Vec2i size;
    glfwGetWindowSize(_window, &size.x, &size.y);
    return size;
  }
  
  /* glfw: retrieves the size, in pixels, of the framebuffer of the specified window */
  Vec2i GetFramebufferSize() const 
  {
    Vec2i size;
    glfwGetFramebufferSize(_window, &size.x, &size.y);
    return size;
  }

  /* glfw: keyboard key */
  int GetKey(uint32_t key) const { return glfwGetKey(_window, key); }

  /* glfw: mouse Key */
  int GetMouseKey(uint32_t key) const { return glfwGetMouseButton(_window, key); }

  /* glfw: mouse position */
  void GetCursorPosition(Vec2d& pos) const { glfwGetCursorPos(_window, &pos.x, &pos.y); }

  /* glfw: value = GLFW_CURSOR_NORMAL | GLFW_CURSOR_HIDDEN | GLFW_CURSOR_DISABLED */
  void SetCursorMode(int value) { glfwSetInputMode(_window, GLFW_CURSOR, value); }

  /* glfw: window position */
  void SetPosition(Vec2i pos) { glfwSetWindowPos(_window, pos.x, pos.y); }

private:
  GLFWwindow* _window;
};
