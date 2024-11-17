#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

struct WindowProps 
{
  vec2i size;
  vec2i position;
  StringView title;

  vec2i aspectRatio;

  bool vsync;

  WindowProps(vec2i size, vec2i position, const char* title, vec2i aspectRatio, bool vsync)
    : size{ size },
      position{ position },
      title{ title }, 
      aspectRatio{ aspectRatio },
      vsync{ vsync }
  {}
};

class WindowManager
{
public:
  using Context = struct GLFWwindow*;

  WindowManager(const WindowManager&) = delete;
  void operator=(const WindowManager&) = delete;

  /**
   * @return
   * The instance of this WindowManager singleton class
   */
  static WindowManager& Get()
  {
    static WindowManager windowManager;
    return windowManager;
  }

  /**
   * @brief
   * Initialize GLFW and OpenGL libraries and create context
   */
  void Initialize(WindowProps props);

  /**
   * @brief
   * Destroy window context and free GLFW resources
   */
  void CleanUp() const;

  /**
   * @brief
   * Processes all pending events
   */
  void PoolEvents() const;

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

  Context GetCurrentContext() const { return _context; }

private:
  WindowManager() = default;
  ~WindowManager() = default;

  Context _context;
};