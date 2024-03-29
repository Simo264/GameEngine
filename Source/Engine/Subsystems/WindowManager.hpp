#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Subsystems/Manager.hpp"

/* ----------------------------------------
      Window manager class built above GLFW
  ---------------------------------------- */

class WindowManager : public Manager<WindowManager>
{
public:
  using Context = struct GLFWwindow*;

  /* Initialize GLFW and OpenGL libraries and create context */
  void Initialize() override;

  /* Destroy window context and free GLFW resources */
  void CleanUp() override;

  /* Processes all pending events */
  void PoolEvents() const;

  /* Close window */
  void Close() const;

  /* Check if window is open */
  bool IsOpen() const;

  /* Return GLFWwindow * object */
  constexpr Context GetContext() const { return _context; }

  /* Set current context with new one */
  constexpr void SetContext(Context newcontext) { _context = newcontext; }

  /* Set window title */
  void SetWindowTitle(const char* title) const;

  /* Swap buffers and poll IO events(keys pressed / released, mouse moved etc.) */
  void SwapWindowBuffers() const;

  /* Set window aspect ratio. e.g 16:9 */
  void SetWindowAspectRatio(int numer, int denom) const;

  /* Get window size */
  Vec2i GetWindowSize() const;
  
  /* Set window resolution */
  void SetWindowSize(int w, int h) const;

  /* Get the size, in pixels, of the framebuffer of the current context */
  Vec2i GetFramebufferSize() const;

  /* Get Keyboard key */
  int GetKey(uint32_t key) const;

  /* Get mouse Key */
  int GetMouseKey(uint32_t key) const; 

  /* Get mouse position */
  Vec2d GetCursorPosition() const;

  /* Set mouse mode: value = GLFW_CURSOR_NORMAL | GLFW_CURSOR_HIDDEN | GLFW_CURSOR_DISABLED */
  void SetCursorMode(int value) const;

  /* Set window position */
  void SetWindowPosition(int x, int y) const;

  /* Set the window vsync */
  void SetWindowVsync(bool b) const;

private:
  Context _context;
};
