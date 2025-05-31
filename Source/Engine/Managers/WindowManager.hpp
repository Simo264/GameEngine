#pragma once

#include "Core/Core.hpp"
#include "Core/DesignPatterns/Singleton.hpp"
#include "Core/Math/Base.hpp"

enum class InputKey : i32
{
  // Lettere
  A = 65,  // GLFW_KEY_A
  B = 66,  // GLFW_KEY_B
  C = 67,  // GLFW_KEY_C
  D = 68,  // GLFW_KEY_D
  E = 69,  // GLFW_KEY_E
  F = 70,  // GLFW_KEY_F
  G = 71,  // GLFW_KEY_G
  H = 72,  // GLFW_KEY_H
  I = 73,  // GLFW_KEY_I
  J = 74,  // GLFW_KEY_J
  K = 75,  // GLFW_KEY_K
  L = 76,  // GLFW_KEY_L
  M = 77,  // GLFW_KEY_M
  N = 78,  // GLFW_KEY_N
  O = 79,  // GLFW_KEY_O
  P = 80,  // GLFW_KEY_P
  Q = 81,  // GLFW_KEY_Q
  R = 82,  // GLFW_KEY_R
  S = 83,  // GLFW_KEY_S
  T = 84,  // GLFW_KEY_T
  U = 85,  // GLFW_KEY_U
  V = 86,  // GLFW_KEY_V
  W = 87,  // GLFW_KEY_W
  X = 88,  // GLFW_KEY_X
  Y = 89,  // GLFW_KEY_Y
  Z = 90,  // GLFW_KEY_Z

  // Numeri
  Num0 = 48,  // GLFW_KEY_0
  Num1 = 49,  // GLFW_KEY_1
  Num2 = 50,  // GLFW_KEY_2
  Num3 = 51,  // GLFW_KEY_3
  Num4 = 52,  // GLFW_KEY_4
  Num5 = 53,  // GLFW_KEY_5
  Num6 = 54,  // GLFW_KEY_6
  Num7 = 55,  // GLFW_KEY_7
  Num8 = 56,  // GLFW_KEY_8
  Num9 = 57,  // GLFW_KEY_9

  // Tasti funzione principali
  Escape = 256,  // GLFW_KEY_ESCAPE
  Enter = 257,  // GLFW_KEY_ENTER
  Tab = 258,  // GLFW_KEY_TAB
  Backspace = 259,  // GLFW_KEY_BACKSPACE
  Space = 32,   // GLFW_KEY_SPACE

  // Frecce direzionali
  Left = 263,  // GLFW_KEY_LEFT
  Right = 262,  // GLFW_KEY_RIGHT
  Down = 264,  // GLFW_KEY_DOWN
  Up = 265,  // GLFW_KEY_UP

  // Modificatori
  LeftShift = 340,  // GLFW_KEY_LEFT_SHIFT
  LeftControl = 341,  // GLFW_KEY_LEFT_CONTROL
  LeftAlt = 342,  // GLFW_KEY_LEFT_ALT
  RightShift = 344,  // GLFW_KEY_RIGHT_SHIFT
  RightControl = 345,  // GLFW_KEY_RIGHT_CONTROL
  RightAlt = 346   // GLFW_KEY_RIGHT_ALT
};
enum class InputMouseKey : i32
{
  Button1 = 0,  // GLFW_MOUSE_BUTTON_1
  Button2 = 1,  // GLFW_MOUSE_BUTTON_2
  Button3 = 2,  // GLFW_MOUSE_BUTTON_3
  Button4 = 3,  // GLFW_MOUSE_BUTTON_4
  Button5 = 4,  // GLFW_MOUSE_BUTTON_5
  Button6 = 5,  // GLFW_MOUSE_BUTTON_6
  Button7 = 6,  // GLFW_MOUSE_BUTTON_7
  Button8 = 7,  // GLFW_MOUSE_BUTTON_8

  Left = Button1,   // Alias for GLFW_MOUSE_BUTTON_1
  Right = Button2,  // Alias for GLFW_MOUSE_BUTTON_2
  Wheel = Button3   // Alias for GLFW_MOUSE_BUTTON_3
};
enum class CursorMode : i32
{
  Normal = 0x00034001,  // GLFW_CURSOR_NORMAL
  Hidden = 0x00034002,  // GLFW_CURSOR_HIDDEN
  Disabled = 0x00034003   // GLFW_CURSOR_DISABLED
};
enum class InputAction : i32
{
  Release = 0,  // GLFW_RELEASE
  Press = 1,  // GLFW_PRESS
  Repeat = 2   // GLFW_REPEAT
};

struct WindowProps 
{
  WindowProps(Vec2I size, Vec2I position, const char* title, Vec2I aspectRatio, bool vsync) :
    size{ size },
    position{ position },
    title{ title },
    aspectRatio{ aspectRatio },
    vsync{ vsync }
  {}

  Vec2I size;
  Vec2I position;
  StringView title;
  Vec2I aspectRatio;
  bool vsync;
};

/**
 * @brief Manages the creation, configuration, and control of GLFW windows and OpenGL contexts.
 * 
 * This class implements the singleton pattern to ensure only one instance of WindowManager
 * exists throughout the application.
 */
class WindowManager : public Singleton<WindowManager>
{
public:
  using Context = struct GLFWwindow*;

  /** @brief Initializes GLFW, creates a window, and sets up the OpenGL context. */
  void Initialize(WindowProps props);
  /** @brief Cleans up GLFW resources by destroying the window context and terminating GLFW. */
  void CleanUp() const;
  /** @brief Processes all pending events in the GLFW event queue. */
  void PoolEvents() const;
  /** @brief Gets the current window context associated with GLFW. */
  Context GetCurrentContext() const { return _context; }
  /** @brief Sets the specified window context as the current one for OpenGL operations. */
  void MakeContextCurrent(Context context);

  /** @brief Closes the currently active window. */
  void Close() const;
  /** @brief Checks whether the window is still open. */
  bool IsOpen() const;
  /** @brief Sets the title of the current window. */
  void SetWindowTitle(StringView title) const;
  /** @brief Swaps the buffers of the current window for rendering. */
  void SwapWindowBuffers() const;
  /** @brief Adjusts the aspect ratio of the window. */
  void SetWindowAspectRatio(i32 numer, i32 denom) const;
  /** @brief Retrieves the dimensions of the current window. */
  Vec2I GetWindowSize() const;
  /** @brief Modifies the size of the current window. */
  void SetWindowSize(i32 w, i32 h) const;
  /** @brief Updates the position of the window on the screen. */
  void SetWindowPosition(i32 x, i32 y) const;
  /** @brief Gets the position of the current window on the screen. */
  Vec2I GetWindowPos() const;
  /** @brief Toggles the vertical synchronization (VSync) for the current window. */
  void SetWindowVsync(bool b) const;
  /** @brief Retrieves the size of the framebuffer for the current window. */
  Vec2I GetFramebufferSize() const;
  /** @brief Returns the state of a specified key in the current window context. */
  InputAction GetKey(InputKey key) const;
  /** @brief Retrieves the state of a specific mouse button in the current window context. */
  InputAction GetMouseKey(InputMouseKey key) const;
  /** @brief Obtains the current position of the cursor relative to the window. */
  Vec2D GetCursorPosition() const;
  /** @brief Sets the cursor's behavior mode (e.g., normal, hidden, disabled). */
  void SetCursorMode(CursorMode mode) const;
  /** @brief Retrieves the version string of the GLFW library in use. */
  const char* GetVersion() const;
  /** @brief Returns the elapsed time (in seconds) since the initialization of GLFW. */
  f64 GetWorldTime() const;

private:
  /** @brief Holds the current GLFW window context associated with this manager. */
  Context _context;
};