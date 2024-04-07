#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Subsystems/Manager.hpp"

/**
 *  Window manager class built above GLFW
 */
class WindowManager : public Manager<WindowManager>
{
public:
  using Context = struct GLFWwindow*;

  /**
   * Initialize GLFW and OpenGL libraries and create context
   */
  void Initialize() override;

  /**
   * Destroy window context and free GLFW resources
   */
  void CleanUp() override;

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

  Vec2i32 GetWindowSize() const;
  
  void SetWindowSize(int w, int h) const;

  Vec2i32 GetFramebufferSize() const;

  int GetKey(uint32_t key) const;

  int GetMouseKey(uint32_t key) const; 

  Vec2d GetCursorPosition() const;

  /**
   * @param value = GLFW_CURSOR_NORMAL | GLFW_CURSOR_HIDDEN | GLFW_CURSOR_DISABLED
   */
  void SetCursorMode(int value) const;

  void SetWindowPosition(int x, int y) const;

  void SetWindowVsync(bool b) const;

  constexpr Context GetContext() const { return _context; }

  constexpr void SetContext(Context newcontext) { _context = newcontext; }

private:
  Context _context;
};
