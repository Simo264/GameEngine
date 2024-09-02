#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/**
 *  Window manager singleton class
 */
class WindowManager
{
public:
  using Context = struct GLFWwindow*;

  WindowManager(const WindowManager&) = delete;
  void operator=(const WindowManager&) = delete;

  /**
   * Return the instance of this WindowManager singleton class
   */
  static WindowManager& Get()
  {
    static WindowManager windowManager;
    return windowManager;
  }

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

  void SetWindowAspectRatio(i32 numer, i32 denom) const;

  vec2i32 GetWindowSize() const;
  
  void SetWindowSize(i32 w, i32 h) const;

  vec2i32 GetFramebufferSize() const;

  i32 GetKey(u32 key) const;

  i32 GetMouseKey(u32 key) const; 

  vec2d GetCursorPosition() const;

  void SetCursorMode(i32 value) const;

  void SetWindowPosition(i32 x, i32 y) const;

  void SetWindowVsync(bool b) const;

  void MakeContextCurrent(Context context);

  constexpr Context GetCurrentContext() const { return _context; }

  constexpr vec2i32 GetAspectRatio() const { return _aspectRatio; }

private:
  WindowManager() = default;
  ~WindowManager() = default;

  Context _context;
  vec2i32 _aspectRatio;
};