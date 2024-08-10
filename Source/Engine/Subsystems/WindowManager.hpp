#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/**
 *  Window manager class built above GLFW
 */
class WindowManager
{
public:
  using Context = struct GLFWwindow*;

  /**
   * Initialize GLFW and OpenGL libraries and create context
   */
  void Initialize();

  /**
   * Destroy window context and free GLFW resources
   */
  void CleanUp() const;

  /**
   * Processes all pending events
   */
  void PoolEvents() const;

  /**
   * Close window
   */
  void Close() const;

  /**
   * Check if window is open
   */
  bool IsOpen() const;

  void SetWindowTitle(const char* title) const;

  void SwapWindowBuffers() const;

  void SetWindowAspectRatio(int numer, int denom) const;

  vec2i32 GetWindowSize() const;
  
  void SetWindowSize(int w, int h) const;

  vec2i32 GetFramebufferSize() const;

  int GetKey(uint32_t key) const;

  int GetMouseKey(uint32_t key) const; 

  vec2d GetCursorPosition() const;

  /**
   * @param value: GLFW_CURSOR_NORMAL or GLFW_CURSOR_HIDDEN or GLFW_CURSOR_DISABLED
   */
  void SetCursorMode(int value) const;

  void SetWindowPosition(int x, int y) const;

  void SetWindowVsync(bool b) const;

  void MakeContextCurrent(Context context);

  constexpr Context GetCurrentContext() const { return _context; }

  constexpr vec2i32 GetAspectRatio() const { return _aspectRatio; }

private:
  Context _context;
  vec2i32 _aspectRatio;
};