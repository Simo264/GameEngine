#pragma once
#include "Core.hh"

class Window
{
public:
  Window() = default;
  ~Window() = default;

  Window(const Window&) = delete;            // delete copy constructor
  Window& operator=(const Window&) = delete; // delete assign op

  // creating a window and context
  void Create(vec2u dim, vec2u pos, const char* title);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  void Destroy();

  // glfw: set close flag TRUE
  void Close() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }

  // glfw: render loop
  bool Loop() const { return !glfwWindowShouldClose(_window); }

  // return GLFWwindow* object
  GLFWwindow* Get() const { return _window; }

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  void SwapBuffers() { glfwSwapBuffers(_window); }

  // glfw: retrieves the size, in pixels, of the framebuffer of the specified window
  void GetFramebufferSize(vec2i& sz) { glfwGetFramebufferSize(_window, &sz.x, &sz.y); }

  // window dimension
  const uint32_t& Width()  const { return _width; }
  const uint32_t& Height() const { return _height; }

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
  void GetPosition(vec2i& pos) { glfwGetWindowPos(_window, &pos.x, &pos.y); }

  void ProcessKeyboardInput();

private:
  GLFWwindow* _window;

  uint32_t _width;
  uint32_t _height;
};
