#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

struct WindowProps 
{
  WindowProps(vec2i size, vec2i position, const char* title, vec2i aspectRatio, bool vsync) :
    size{ size },
    position{ position },
    title{ title },
    aspectRatio{ aspectRatio },
    vsync{ vsync }
  {}

  vec2i size;
  vec2i position;
  StringView title;
  vec2i aspectRatio;
  bool vsync;
};

/**
 * @class WindowManager
 * 
 * @brief Manages the creation, configuration, and control of GLFW windows and OpenGL contexts.
 * 
 * This class implements the singleton pattern to ensure only one instance of WindowManager
 * exists throughout the application.
 */
class WindowManager
{
public:
  /** @brief Deleted copy constructor to enforce singleton pattern. */
  WindowManager(const WindowManager&) = delete;
  /** @brief Deleted assignment constructor to enforce singleton pattern. */
  void operator=(const WindowManager&) = delete;
  
  using Context = struct GLFWwindow*;

  /** @brief Retrieves the singleton instance of WindowManager. */
  static WindowManager& Get()
  {
    static WindowManager windowManager;
    return windowManager;
  }

  /** @brief Initializes GLFW, creates a window, and sets up the OpenGL context. */
  void Initialize(WindowProps props);
  /** @brief Cleans up GLFW resources by destroying the window context and terminating GLFW. */
  void CleanUp() const;
  /** @brief Processes all pending events in the GLFW event queue. */
  void PoolEvents() const;
  
  Context GetCurrentContext() const { return _context; }

  void Close() const;
  bool IsOpen() const;
  void SetWindowTitle(const char* title) const;
  void SwapWindowBuffers() const;
  void SetWindowAspectRatio(i32 numer, i32 denom) const;
  vec2i GetWindowSize() const;
  void SetWindowSize(i32 w, i32 h) const;
  vec2i GetWindowPos() const;
  vec2i GetFramebufferSize() const;
  i32 GetKey(u32 key) const;
  i32 GetMouseKey(u32 key) const; 
  vec2d GetCursorPosition() const;
  void SetCursorMode(i32 value) const;
  void SetWindowPosition(i32 x, i32 y) const;
  void SetWindowVsync(bool b) const;
  void MakeContextCurrent(Context context);
  const char* GetVersion() const;

private:
  WindowManager() = default;
  ~WindowManager() = default;

  Context _context;
};